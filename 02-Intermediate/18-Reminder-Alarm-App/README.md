# Project 18: Reminder-Alarm-App

## 1. Project Overview
`Reminder-Alarm-App` is a multi-threaded task alarm and reminder scheduling daemon. It executes non-busy-waiting condition-variable sleep loops, supports recurring interval alarms and snooze deltas, and emits acoustic alerts via platform APIs.

## 2. Problem Statement
Time-sensitive alarms and task reminder applications require accurate background sleep timing that consumes near-zero CPU cycles while idle, paired with thread-safe queue mutations when new alarms or snooze actions are scheduled concurrently.

## 3. Objectives
- Implement a worker thread using `std::condition_variable::wait_until` to eliminate busy polling loops.
- Support recurring reminder intervals and snooze extensions.
- Provide acoustic sound alerts (`MessageBeep` on Windows) on alarm expiration.
- Expose clear callbacks to allow both GUI notifications and quiet unit testing.

## 4. Features
- **Zero-CPU Idle Sleep**: Synchronized via condition variable timed waits (`wait_until`).
- **Snooze & Dismiss Logic**: Postpone active alarms by parameterized duration increments.
- **Recurring Alarms**: Automatically reschedule periodic interval reminders.
- **Cross-Platform Audio**: Integrated OS acoustic notification cues.

## 5. Technology Stack
- **Language**: C++17
- **Compiler**: GCC 11+, Clang 14+, MSVC 2022
- **Build System**: CMake 3.20+
- **Concurrency**: `std::thread`, `std::mutex`, `std::condition_variable`, `std::atomic`
- **Testing**: doctest 2.4+ via CTest
- **Platform Sound**: Win32 `MessageBeep` (linked to `user32` on Windows)

## 6. Folder Structure
```
18-Reminder-Alarm-App/
├── include/
│   └── AlarmScheduler.hpp
├── src/
│   ├── AlarmScheduler.cpp
│   └── main.cpp
├── tests/
│   └── test_alarm_scheduler.cpp
├── CMakeLists.txt
├── README.md
└── .gitignore
```

## 7. Installation / Prerequisites
- CMake 3.20+
- C++17 compliant compiler
- Threading support (POSIX threads / Win32)

## 8. Build and Run Instructions
### Windows & Linux
```bash
cmake -S . -B build -G Ninja
cmake --build build

# Headless scenario verification:
./build/ReminderAlarmApp --test-run
```

## 9. Testing Instructions
```bash
ctest --test-dir build --output-on-failure
```

## 10. Console Output Sample
```
=================================================================
                REMINDER & ALARM SCHEDULER DAEMON                
=================================================================
[Test-Run Mode] Scheduling short timer (1s) and testing trigger...
Scheduled alarm ID 1 for 1 second in the future.
Pending alarm count: 1
  -> Alarm event triggered: "Daily Standup Reminder" (ID: 1)
Alarm fired status: SUCCESS
Pending count after trigger: 0
[Test-Run Mode] Completed successfully.
```

## 11. Known Limitations
- Current sound triggering relies on the standard system alert beep; custom MP3/WAV playback requires an audio decoder pipeline.

## 12. Testing Status
- **Built**: Yes (Clean compilation under GCC 16.1.0 on Windows, 0 warnings).
- **Run**: Verified `--test-run` and background worker timer firing.
- **Tested**: 2 multi-subcase test suites passing under CTest.
