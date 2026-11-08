#pragma once

#include <iostream>
#include <vector>
#include <queue>
#include <memory>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <cstdint>
#include <cstring>
#include <optional>
#include <string_view>

// Hardware Bitwise Motor Flags
constexpr uint8_t BIT_POWER_ON  = (1 << 0);
constexpr uint8_t BIT_OVERTEMP  = (1 << 1);
constexpr uint8_t BIT_ESTOP     = (1 << 2);

// Hardware Wire Frame Protocol (Packed 1-Byte Alignment)
#pragma pack(push, 1)
struct AMRHardwareTelemetry {
    uint8_t  sensor_id;
    int16_t  velocity_rpm;
    int16_t  temperature_c;
    uint8_t  status_flags;
    uint32_t timestamp_ms;
};
#pragma pack(pop)

// Thread-Safe Zero-Copy Telemetry Queue
class TelemetryChannel {
public:
    void push(std::unique_ptr<AMRHardwareTelemetry> packet);
    std::unique_ptr<AMRHardwareTelemetry> pop();
    void shutdown();

private:
    std::queue<std::unique_ptr<AMRHardwareTelemetry>> queue_;
    std::mutex mtx_;
    std::condition_variable cv_;
    bool finished_ = false;
};

// Function Declarations
uint16_t compute_crc16(const uint8_t* data, size_t length);
std::optional<AMRHardwareTelemetry> parse_hardware_frame(const std::vector<uint8_t>& raw_bytes);