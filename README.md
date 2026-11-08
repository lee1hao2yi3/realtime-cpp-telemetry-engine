# Real-Time C++ Telemetry & Hardware Engine

A low-latency, multi-threaded telemetry processing engine written in C++20, designed for embedded Linux robotics systems, Autonomous Mobile Robots (AMRs), and industrial hardware interfaces.

## 🚀 Key Engineering Features
* **Modern C++20 Concurrency:** Implements RAII memory management via `std::unique_ptr`, condition variables (`std::condition_variable`), and zero-copy thread synchronization with `std::move`.
* **Hardware Wire Protocols:** Binary struct packing (`#pragma pack(1)`) to enforce 1-byte alignment matching CAN bus / UART wire formats.
* **Deterministic Bitwise Masking:** Direct bitwise operations (`&`, `|`, `~`) for status register parsing and safety state checking.
* **CRC-16 Data Integrity:** CCITT CRC-16 checksum validation for raw binary packet parsing and corrupted frame rejection.
* **Zero Spin-Locking:** Event-driven producer-consumer pipeline eliminating CPU idling.

## 🛠️ Build & Execution Instructions

### Prerequisites
* CMake 3.16+
* C++20 compliant compiler (`clang++` or `g++`)

### Build Steps
```bash
# Clone the repository
git clone [https://github.com/YOUR_GITHUB_USERNAME/realtime-cpp-telemetry-engine.git](https://github.com/YOUR_GITHUB_USERNAME/realtime-cpp-telemetry-engine.git)
cd realtime-cpp-telemetry-engine

# Build executable
mkdir build && cd build
cmake ..
make

# Run binary
./telemetry_engine