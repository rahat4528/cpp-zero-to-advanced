# 12-Internet-Speed-Tester

A network throughput benchmarking utility and HTTP download speed tester built with modern C++17.

---

## 1. Project Overview
Internet Speed Tester measures network download speeds, round-trip connection latency (ping), and bandwidth throughput (in both Mbps and MB/s). It includes a deterministic offline mock test mode to ensure test verification in sandboxed or offline environments.

## 2. Problem Statement
Measuring internet connection quality requires accurately timing chunked HTTP data streams, calculating bitrates without integer overflow, measuring initial handshake latency, and reporting results with clear unit representations.

## 3. Objectives
- Measure round-trip handshake latency using `<chrono>`.
- Ingest HTTP byte streams and calculate download speed in Mbps (Megabits per second) and MB/s (Megabytes per second).
- Provide an offline deterministic mock test mode (`--mock`) for test suites.
- Format structured human-readable terminal reports.

## 4. Features
- **Accurate Timing**: High-resolution clock (`std::chrono::steady_clock`) for microsecond-accurate bandwidth calculation.
- **Dual Speed Metrics**: Displays both Megabits/s (telecom standard) and Megabytes/s (filesystem transfer standard).
- **Latency / Ping**: Measures connection setup round-trip time.
- **Offline Mock Engine**: `--mock` simulates network transfers, enabling offline unit testing.

## 5. Technology Stack
- **Language**: C++17 (`<chrono>`, `std::thread`, `std::ostringstream`)
- **Networking**: WinINet native network API on Windows; mock network engine for cross-platform offline testing.
- **Build System**: CMake 3.20+
- **Testing Framework**: doctest with CTest
- **Database Needed**: No.
- **Internet / External Service Needed**: Yes, for live network testing (`http://speedtest.tele2.net/1MB.zip`). No internet required when running in `--mock` mode or during CTest unit tests.

## 6. Folder Structure
```
12-Internet-Speed-Tester/
├── include/
│   └── SpeedTester.h          # API and throughput calculation structures
├── src/
│   ├── SpeedTester.cpp        # Network timing and download implementation
│   └── main.cpp               # CLI driver and report formatter
├── tests/
│   └── test_speed_tester.cpp  # doctest unit test suite
├── CMakeLists.txt             # CMake build configuration
├── .gitignore                 # Build ignore rules
└── README.md                  # Project documentation
```

## 7. Installation / Prerequisites
- GCC 11+, Clang 14+, or MSVC 2022
- CMake 3.20+ and Ninja (or Make)

## 8. Build and Run Instructions

### Windows (PowerShell)
```powershell
cmake -S . -B build -G Ninja
cmake --build build

# Offline mock test run
.\build\SpeedTester.exe --mock

# Live speed test against remote mirror
.\build\SpeedTester.exe --url http://speedtest.tele2.net/1MB.zip
```

### Linux / macOS
```bash
cmake -S . -B build -G Ninja
cmake --build build

./build/SpeedTester --mock
```

## 9. Testing Instructions
Run tests with CTest:
```bash
ctest --test-dir build --output-on-failure
```

## 10. Demo / Console Output Sample
```
$ ./build/SpeedTester --mock
Starting Speed Test (Mock Mode)...
========================================
       INTERNET SPEED TEST REPORT       
========================================
Status         : SUCCESS
Test Mode      : Mock / Simulated
Ping / Latency : 25.10 ms
Data Ingested  : 9.54 MB (10000000 bytes)
Test Duration  : 0.10 s
----------------------------------------
Download Speed : 792.08 Mbps
Throughput     : 94.42 MB/s
========================================
```

## 11. Known Limitations
- Download speed only (upload speed requires server-side HTTP POST handler).
- Non-Windows environments default to simulated mock mode unless libcurl package is explicitly linked.

## 12. Testing Status
- **Built**: Yes, compiled cleanly with GCC 16.1.0 using `-Wall -Wextra -Wpedantic`.
- **Run**: Yes, verified both mock and live download modes.
- **Tested**: Yes, 3 unit test suites passing via CTest.
- **Hardware/Credentials Limitations**: Live test requires active internet; mock test runs 100% offline.
