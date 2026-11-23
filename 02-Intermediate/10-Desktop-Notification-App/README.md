# Project 10: Desktop-Notification-App

## 1. Project Overview
`Desktop-Notification-App` is a cross-platform desktop notification scheduling and alert dispatcher. It provides thread-safe prioritized alert queues (`Low`, `Normal`, `High`, `Critical`), delayed notification timers, acoustic alert cues via OS audio APIs, and life-cycle dismissals.

## 2. Problem Statement
Desktop systems require background notification services to manage asynchronous events, reminders, and alerts without blocking UI threads, with support for priority ordering, delayed triggers, and audible warnings.

## 3. Objectives
- Architect a multi-threaded notification dispatcher with thread-safe queue manipulation via `std::mutex`.
- Implement priority hierarchies and scheduled delays using background worker threads.
- Integrate OS native auditory signals (`MessageBeep` on Windows) for elevated alerts.
- Decouple alert notification callbacks from core queue logic to support CLI, system tray, or native desktop notification bindings.

## 4. Features
- **Priority Tiering**: `Low`, `Normal`, `High`, and `Critical` priorities with tailored dispatch behavior.
- **Delayed Timers**: Asynchronously posts notifications after a designated time interval.
- **OS Audio Integration**: Emits platform-specific audio feedback on High and Critical events.
- **Headless Test Support**: Includes `--test-run` flag and customizable callbacks for quiet unit test assertions.

## 5. Technology Stack
- **Language**: C++17
- **Compiler**: GCC 11+, Clang 14+, MSVC 2022
- **Build System**: CMake 3.20+
- **Threading**: C++17 `std::thread`, `std::mutex`, `std::atomic`
- **Testing**: doctest 2.4+ via CTest
- **GUI / Tray Rationale**: Structured with a decoupled event queue model. On Windows, links `user32` for acoustic system alerts; formatted console visual banners are used for universal cross-platform operation without requiring heavy Qt GUI toolchains.

## 6. Folder Structure
```
10-Desktop-Notification-App/
├── include/
│   └── NotificationManager.hpp
├── src/
│   ├── NotificationManager.cpp
│   └── main.cpp
├── tests/
│   └── test_notifications.cpp
├── CMakeLists.txt
├── README.md
└── .gitignore
```

## 7. Installation / Prerequisites
- CMake 3.20+
- C++17 compliant compiler
- Threading support (POSIX threads / Win32 threads)

## 8. Build and Run Instructions
### Windows & Linux
```bash
cmake -S . -B build -G Ninja
cmake --build build

# Launch application console:
./build/DesktopNotificationApp

# Run headless automated verification:
./build/DesktopNotificationApp --test-run
```

## 9. Testing Instructions
```bash
ctest --test-dir build --output-on-failure
```

## 10. Console Output Sample
```
=================================================================
             DESKTOP NOTIFICATION DISPATCHER APP                 
=================================================================
[Test-Run Mode] Dispatching priority notifications and testing lifecycle...

[DESKTOP NOTIFICATION - NORMAL]
  Title  : Calendar Event
  Message: Team sync in 10 minutes


[DESKTOP NOTIFICATION - HIGH]
  Title  : Low Battery
  Message: 15% remaining. Plug in charger.


[DESKTOP NOTIFICATION - CRITICAL]
  Title  : Disk Space Critical
  Message: Root partition has < 500 MB remaining!

Dispatched 3 notifications: IDs 1, 2, 3
Active notification count: 3
Dismissed ID 1. Active remaining: 2
[Test-Run Mode] Execution completed successfully.
```

## 11. Known Limitations
- Rich visual toast notifications with image icons require platform-specific WinRT / D-Bus notification daemons.

## 12. Testing Status
- **Built**: Yes (Clean compilation under GCC 16.1.0 on Windows, 0 warnings).
- **Run**: Verified interactive launch and `--test-run` headless mode.
- **Tested**: 3 test suites covering queue lifecycle, priority conversions, and callback execution passing under CTest.
