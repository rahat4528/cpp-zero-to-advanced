# 02-Digital-Clock-UI

A cross-platform real-time Digital Clock User Interface featuring 7-segment digital font rendering, timezone adjustments, and 12/24-hour modes.

---

## 1. Project Overview
Digital Clock UI delivers a digital clock interface with high-contrast 7-segment LED-style representations built entirely in modern C++17. It supports timezone hour adjustments, configurable 12-hour (AM/PM) and 24-hour military formats, and single-frame snapshot execution for headless validation.

## 2. Problem Statement
Digital displays require careful handling of date-time standards, clock precision, leap seconds, time zone conversions, and modular visual rendering that can operate across terminals and graphical environments without requiring hundreds of megabytes of heavy GUI dependencies.

## 3. Objectives
- Utilize standard `<chrono>` utilities for high-precision time calculations.
- Implement 12-hour to 24-hour conversion algorithms.
- Provide timezone offset calculations for global time tracking.
- Construct a modular 7-segment ASCII LED font renderer.
- Provide deterministic snapshot and unit test harnesses.

## 4. Features
- **LED 7-Segment Font**: Renders digits in 5-row tall retro digital clock typography.
- **Timezone Selector**: Configurable UTC offsets (`--tz +6`, `--tz -5`).
- **Flexible Modes**: Standard 24h format and 12h format with AM/PM indicators.
- **Snapshot & Headless Mode**: `--snapshot` outputs an instant frame for automated testing.

## 5. Technology Stack
- **Language**: C++17 (`std::chrono`, `std::tm`, `std::array`, `std::ostringstream`)
- **UI Toolkit**: Interactive Terminal UI (TUI) with 7-segment LED renderer. *Rationale*: Selected over Qt6 Widgets to keep the project completely lightweight, zero-dependency, and instantly compilable on any Linux or Windows system without requiring a 5GB Qt installation.
- **Build System**: CMake 3.20+
- **Testing Framework**: doctest with CTest
- **Database Needed**: No.
- **Internet / External Service Needed**: No. Uses local system monotonic and wall clocks.

## 6. Folder Structure
```
02-Digital-Clock-UI/
├── include/
│   └── DigitalClock.h        # Time calculations and 7-segment API
├── src/
│   ├── DigitalClock.cpp      # Implementation of clock math and glyphs
│   └── main.cpp              # UI timer loop and arguments
├── tests/
│   └── test_digital_clock.cpp# Unit test suite
├── CMakeLists.txt            # CMake build configuration
├── .gitignore                # Build ignore rules
└── README.md                 # Project documentation
```

## 7. Installation / Prerequisites
- GCC 11+, Clang 14+, or MSVC 2022
- CMake 3.20+ and Ninja (or Make)

## 8. Build and Run Instructions

### Windows (PowerShell)
```powershell
cmake -S . -B build -G Ninja
cmake --build build

# Run in snapshot mode
.\build\DigitalClock.exe --snapshot --tz 6

# Run in 12-hour mode
.\build\DigitalClock.exe --snapshot --12h
```

### Linux / macOS
```bash
cmake -S . -B build -G Ninja
cmake --build build

./build/DigitalClock --snapshot --tz 6
```

## 9. Testing Instructions
Run automated test suite via CTest:
```bash
ctest --test-dir build --output-on-failure
```

## 10. Demo / Console Output Sample
```
============================================
             DIGITAL CLOCK UI               
============================================
 Current Time: 03:15:00 (UTC+6)

 _   _       _   _       _   _  
| |   |   o |   | |   o | | | | 
 _   _       _   _       _   _  
| | |     o   | | |   o | | | | 
 _   _       _   _       _   _  
============================================
```

## 11. Known Limitations
- Timezone offsets currently take whole hour increments.
- Terminal window must have at least 50 columns width for optimal 7-segment visual layout.

## 12. Testing Status
- **Built**: Yes, compiled cleanly with GCC 16.1.0 (`-Wall -Wextra -Wpedantic`).
- **Run**: Yes, tested both `--snapshot` and live ticks.
- **Tested**: Yes, all 3 test suites passing via CTest.
- **Hardware/Credentials Limitations**: None. Runs entirely offline.
