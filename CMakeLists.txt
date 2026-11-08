cmake_minimum_required(VERSION 3.16)
project(RealTimeTelemetryEngine VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Include header search paths
include_directories(include)

# Compile telemetry engine executable
add_executable(telemetry_engine
    src/main.cpp
    src/telemetry_engine.cpp
)

# Compiler flags for warning enforcement and strict execution
target_compile_options(telemetry_engine PRIVATE -Wall -Wextra -Wpedantic)