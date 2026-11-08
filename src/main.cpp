#include "telemetry_engine.hpp"
#include <chrono>

void hardware_driver_producer(TelemetryChannel& channel) {
    for (uint8_t id = 1; id <= 3; ++id) {
        std::this_thread::sleep_for(std::chrono::milliseconds(50));

        AMRHardwareTelemetry raw_frame{
            id,
            static_cast<int16_t>(1200 + id * 50),
            static_cast<int16_t>(35 + id),
            static_cast<uint8_t>(BIT_POWER_ON),
            static_cast<uint32_t>(1000 + id * 50) // Explicit cast to uint32_t
        };

        // Pack frame + append CRC16
        std::vector<uint8_t> bus_packet(sizeof(AMRHardwareTelemetry) + sizeof(uint16_t));
        std::memcpy(bus_packet.data(), &raw_frame, sizeof(AMRHardwareTelemetry));
        uint16_t crc = compute_crc16(bus_packet.data(), sizeof(AMRHardwareTelemetry));
        std::memcpy(bus_packet.data() + sizeof(AMRHardwareTelemetry), &crc, sizeof(uint16_t));

        auto parsed = parse_hardware_frame(bus_packet);
        if (parsed) {
            auto pkt = std::make_unique<AMRHardwareTelemetry>(*parsed);
            std::cout << "[HARDWARE DRIVER 🚗] Emitted Packet for Sensor #" << static_cast<int>(pkt->sensor_id) << "\n";
            channel.push(std::move(pkt));
        }
    }
    channel.shutdown();
}

void ai_navigation_consumer(TelemetryChannel& channel) {
    while (true) {
        auto pkt = channel.pop();
        if (!pkt) break;

        bool is_powered = (pkt->status_flags & BIT_POWER_ON) != 0;
        std::cout << "  [AMR NAV CORE ⚡] Processed Motor #" << static_cast<int>(pkt->sensor_id)
                  << " | Speed: " << pkt->velocity_rpm << " RPM"
                  << " | Temp: " << pkt->temperature_c << "°C"
                  << " | Powered: " << (is_powered ? "YES" : "NO") << "\n";
    }
    std::cout << "  [AMR NAV CORE ⚡] Pipeline safely drained and terminated.\n";
}

int main() {
    std::cout << "=====================================================\n";
    std::cout << "   MONTH 1 CAPSTONE: REAL-TIME AMR TELEMETRY ENGINE  \n";
    std::cout << "=====================================================\n\n";

    TelemetryChannel channel;

    std::thread producer(hardware_driver_producer, std::ref(channel));
    std::thread consumer(ai_navigation_consumer, std::ref(channel));

    producer.join();
    consumer.join();

    std::cout << "\n=====================================================\n";
    std::cout << "[SUCCESS] Month 1 Core C++ & Hardware Systems Verified.\n";
    std::cout << "=====================================================\n";
    return 0;
}