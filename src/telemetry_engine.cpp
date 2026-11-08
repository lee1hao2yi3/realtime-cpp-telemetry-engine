#include "telemetry_engine.hpp"

// Thread-Safe Queue Implementation
void TelemetryChannel::push(std::unique_ptr<AMRHardwareTelemetry> packet) {
    {
        std::lock_guard<std::mutex> lock(mtx_);
        queue_.push(std::move(packet));
    }
    cv_.notify_one();
}

std::unique_ptr<AMRHardwareTelemetry> TelemetryChannel::pop() {
    std::unique_lock<std::mutex> lock(mtx_);
    cv_.wait(lock, [this]() { return !queue_.empty() || finished_; });

    if (queue_.empty() && finished_) return nullptr;

    auto packet = std::move(queue_.front());
    queue_.pop();
    return packet;
}

void TelemetryChannel::shutdown() {
    {
        std::lock_guard<std::mutex> lock(mtx_);
        finished_ = true;
    }
    cv_.notify_all();
}

// CCITT CRC-16 Calculation
uint16_t compute_crc16(const uint8_t* data, size_t length) {
    uint16_t crc = 0xFFFF;
    for (size_t i = 0; i < length; ++i) {
        crc ^= static_cast<uint16_t>(data[i]) << 8;
        for (int bit = 0; bit < 8; ++bit) {
            if (crc & 0x8000) {
                crc = (crc << 1) ^ 0x1021;
            } else {
                crc <<= 1;
            }
        }
    }
    return crc;
}

// Zero-Copy Hardware Frame Parsing & Validation
std::optional<AMRHardwareTelemetry> parse_hardware_frame(const std::vector<uint8_t>& raw_bytes) {
    constexpr size_t expected_size = sizeof(AMRHardwareTelemetry) + sizeof(uint16_t);
    if (raw_bytes.size() < expected_size) return std::nullopt;

    // Checksum validation
    uint16_t rx_crc;
    std::memcpy(&rx_crc, raw_bytes.data() + sizeof(AMRHardwareTelemetry), sizeof(uint16_t));
    uint16_t calc_crc = compute_crc16(raw_bytes.data(), sizeof(AMRHardwareTelemetry));

    if (rx_crc != calc_crc) return std::nullopt;

    AMRHardwareTelemetry telemetry;
    std::memcpy(&telemetry, raw_bytes.data(), sizeof(AMRHardwareTelemetry));
    return telemetry;
}