# 20-Stopwatch-Timer-GUI

A high-resolution millisecond precision Stopwatch and Lap Timer application built with modern C++17.

---

## 1. Project Overview
Stopwatch Timer GUI / TUI measures durations using standard steady monotonic clocks (`std::chrono::steady_clock`). It supports start, pause, resume, split lap recording, lap delta analysis (detecting fastest and slowest laps), and clean terminal UI formatting.

## 2. Problem Statement
Accurate performance timing requires immune monotonic clocks unaffected by system wall-clock time changes or NTP updates, robust pause/resume elapsed state accumulation, and statistical lap comparison.

## 3. Objectives
- Utilize `std::chrono::steady_clock` to prevent clock skew.
- Manage timer states (`Stopped`, `Running`, `Paused`).
- Record split times and relative lap deltas.
- Identify fastest and slowest split times.
- Deliver automated headless testing via `--scripted`.

## 4. Features
- **Millisecond Precision**: Formats to `HH:MM:SS.mmm`.
- **Lap Tracking**: Tracks split durations and individual lap intervals.
- **Fastest / Slowest Tags**: Highlights peak performance laps in the output report.
- **State Controls**: Seamlessly transitions between Start, Pause, Resume, Stop, and Reset.

## 5. Technology Stack
- **Language**: C++17 (`<chrono>`, `std::vector`, `std::string`, `std::ostringstream`)
- **UI Toolkit**: Interactive Terminal UI (TUI). *Rationale*: Selected to maintain zero external dependencies and fast compilation across all systems without requiring 5GB of Qt6 binaries.
- **Build System**: CMake 3.20+
- **Testing Framework**: doctest with CTest
- **Database Needed**: No.
- **Internet / External Service Needed**: No.

## 6. Folder Structure
```
20-Stopwatch-Timer-GUI/
├── include/
│   └── StopwatchTimer.h        # LapRecord and StopwatchTimer API
├── src/
│   ├── StopwatchTimer.cpp      # Monotonic chrono math and lap logic
│   └── main.cpp                # Terminal loop and CLI driver
├── tests/
│   └── test_stopwatch_timer.cpp# doctest unit test suite
├── CMakeLists.txt              # CMake build configuration
├── .gitignore                  # Build ignore rules
└── README.md                   # Project documentation
```

## 7. Installation / Prerequisites
- GCC 11+, Clang 14+, or MSVC 2022
- CMake 3.20+ and Ninja (or Make)

## 8. Build and Run Instructions

### Windows (PowerShell)
```powershell
cmake -S . -B build -G Ninja
cmake --build build

# Automated scripted test
.\build\StopwatchTimer.exe --scripted "start;sleep:100;lap;sleep:150;lap;sleep:80;stop"
```

### Linux / macOS
```bash
cmake -S . -B build -G Ninja
cmake --build build

./build/StopwatchTimer --scripted "start;sleep:100;lap;stop"
```

## 9. Testing Instructions
Run tests with CTest:
```bash
ctest --test-dir build --output-on-failure
```

## 10. Demo / Console Output Sample
```
$ ./build/StopwatchTimer --scripted "start;sleep:100;lap;sleep:150;lap;sleep:80;stop"
=== Stopwatch Timer Automated Test Run ===
>> Stopwatch STARTED
>> LAP #1 Recorded: 00:00:00.101 (Total: 00:00:00.101)
>> LAP #2 Recorded: 00:00:00.151 (Total: 00:00:00.252)
>> Stopwatch STOPPED at: 00:00:00.333

Final Elapsed Time: 00:00:00.333

----------------------------------------------------
LAP #   SPLIT TIME        LAP TIME        TAG
----------------------------------------------------
1       00:00:00.101      00:00:00.101    [FASTEST]
2       00:00:00.252      00:00:00.151    [SLOWEST]
----------------------------------------------------
```

## 11. Known Limitations
- Terminal refresh rates are subject to terminal emulator rendering latency.
- Audio bell notifications are not included (featured in `02-Intermediate/18-Reminder-Alarm-App`).

## 12. Testing Status
- **Built**: Yes, compiled cleanly with GCC 16.1.0 using `-Wall -Wextra -Wpedantic`.
- **Run**: Yes, verified both interactive and automated scripted runs.
- **Tested**: Yes, 5 unit test suites passing via CTest.
- **Hardware/Credentials Limitations**: None.
