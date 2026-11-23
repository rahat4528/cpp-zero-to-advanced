# 13-Calendar-Reminder-App

A Calendar and Event Reminder Application with monthly grid visualization, Zeller's congruence date arithmetic, and disk persistence built with modern C++17.

---

## 1. Project Overview
Calendar Reminder App renders monthly calendar views in the terminal, computes accurate day-of-week offsets, tracks scheduled reminders with priority tags, and persists event queues to disk.

## 2. Problem Statement
Personal scheduling tools require date mathematics (accounting for leap years, variable month lengths, century rules), event prioritization, chronological sorting, and reliable text serialization without heavy external calendar dependencies.

## 3. Objectives
- Implement astronomical Gregorian calendar rules (Zeller's congruence for day-of-week).
- Render formatted monthly calendar ASCII grids.
- Schedule, complete, and delete reminders with priority levels.
- Persist reminders in structured pipe-delimited text files.

## 4. Features
- **Accurate Leap Year Logic**: Full century handling (\(400\)-year and \(100\)-year rules).
- **Monthly Grid**: Clean 7-column calendar matrix.
- **Chronological Sorting**: Reminders automatically sort by date and time.
- **Status Tracking**: Toggle reminders between pending and completed.
- **Scriptable CLI**: Automated flags for testing (`--month`, `--add`, `--list`, `--complete`).

## 5. Technology Stack
- **Language**: C++17 (`<chrono>`, `std::vector`, `std::fstream`, `std::string`)
- **Build System**: CMake 3.20+
- **Testing Framework**: doctest with CTest
- **Database Needed**: No. Plaintext file storage (`reminders.txt`).
- **Internet / External Service Needed**: No.

## 6. Folder Structure
```
13-Calendar-Reminder-App/
├── include/
│   └── CalendarReminder.h       # CalendarHelper and ReminderManager APIs
├── src/
│   ├── CalendarReminder.cpp     # Zeller's congruence, calendar math, file I/O
│   └── main.cpp                 # CLI driver and calendar formatter
├── tests/
│   └── test_calendar_reminder.cpp # doctest unit tests
├── CMakeLists.txt               # CMake build configuration
├── .gitignore                   # Build ignore rules
└── README.md                    # Project documentation
```

## 7. Installation / Prerequisites
- GCC 11+, Clang 14+, or MSVC 2022
- CMake 3.20+ and Ninja (or Make)

## 8. Build and Run Instructions

### Windows (PowerShell)
```powershell
cmake -S . -B build -G Ninja
cmake --build build

# Render monthly calendar
.\build\CalendarReminder.exe --month 2026-11

# Add reminder
.\build\CalendarReminder.exe --file reminders.txt --add "2026-11-25|14:30|Engineering Review|High"

# List reminders
.\build\CalendarReminder.exe --file reminders.txt --list
```

### Linux / macOS
```bash
cmake -S . -B build -G Ninja
cmake --build build

./build/CalendarReminder --month 2026-11
```

## 9. Testing Instructions
Run tests with CTest:
```bash
ctest --test-dir build --output-on-failure
```

## 10. Demo / Console Output Sample
```
$ ./build/CalendarReminder --month 2026-11
=====================================
           November 2026
=====================================
  Su   Mo   Tu   We   Th   Fr   Sa   
-------------------------------------
   1    2    3    4    5    6    7 
   8    9   10   11   12   13   14 
  15   16   17   18   19   20   21 
  22   23   24   25   26   27   28 
  29   30 
=====================================
```

## 11. Known Limitations
- Timezone offsets default to local clock.
- Does not currently trigger background audio alarms (see `02-Intermediate/18-Reminder-Alarm-App` for background threaded audio alarms).

## 12. Testing Status
- **Built**: Yes, compiled with GCC 16.1.0 using `-Wall -Wextra -Wpedantic`.
- **Run**: Yes, verified calendar grid generation, reminder scheduling, and file saving.
- **Tested**: Yes, 5 unit test suites passing via CTest.
- **Hardware/Credentials Limitations**: None.
