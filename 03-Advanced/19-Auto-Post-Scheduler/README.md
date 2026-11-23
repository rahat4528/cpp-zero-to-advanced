# 19-Auto-Post-Scheduler

## 1. Project Overview
19-Auto-Post-Scheduler is an automated social media and webhook content scheduling application developed in modern C++17. Backed by a transactional SQLite database and an asynchronous daemon worker thread with condition variables, it provides durable job persistence across process lifecycles, exponential backoff retries, and a pluggable channel backend architecture (`IPostingBackend`).

## 2. Problem Statement
Automated social distribution pipelines face network flakiness, rate limits, and server restarts. In-memory queue designs risk catastrophic data loss on crash or reboot. Robust scheduling requires durable disk persistence, atomic state transitions (`PENDING`, `DISPATCHING`, `PUBLISHED`, `FAILED`), and clean backend decoupling.

## 3. Objectives
- Implement relational SQLite3 job storage with epoch timestamp indexing.
- Design an abstract plugin interface (`IPostingBackend`) to decouple channel dispatch mechanics (Twitter, LinkedIn, Webhooks) from scheduling.
- Implement an asynchronous worker thread using `std::condition_variable::wait_for` to minimize CPU polling cycles.
- Implement exponential backoff and retry counters upon dispatch failure.
- Provide automated headless CI verification via `--test-run` and doctest unit tests.

## 4. Features
- **ACID Job Durability**: Jobs are committed to SQLite before dispatch scheduling.
- **Pluggable Channel Adapters**: Easily plug custom REST, Webhook, or mock adapters implementing `IPostingBackend`.
- **Fault-Tolerant Retries**: Failed dispatches increment retry counts and backoff future attempts until reaching maximum threshold limits.
- **Cooperative Worker Teardown**: Daemon thread terminates gracefully upon signal with condition variable notification.

## 5. Technology Stack
- **Language**: C++17 (`std::thread`, `std::mutex`, `std::condition_variable`, `std::vector`, `std::string`, `std::chrono`).
- **Database**: SQLite3 via C API (`sqlite3_prepare_v2`, `sqlite3_step`).
- **External Services**: None required by default (mock channel dispatchers included).
- **Testing**: `doctest` unit testing framework.

## 6. Folder Structure
```
19-Auto-Post-Scheduler/
├── CMakeLists.txt
├── README.md
├── .gitignore
├── include/
│   ├── IPostingBackend.hpp
│   ├── PostDatabase.hpp
│   └── PostScheduler.hpp
├── src/
│   ├── PostDatabase.cpp
│   ├── PostScheduler.cpp
│   └── main.cpp
└── tests/
    └── test_post_scheduler.cpp
```

## 7. Installation / Prerequisites
- CMake 3.20 or newer.
- GCC 11+, Clang 14+, or MSVC 2022.
- SQLite3 development library.
- doctest headers.

## 8. Build and Run Instructions
### Windows & Linux Build
```bash
cmake -S . -B build -G Ninja
cmake --build build
```

### Running the Application
```bash
./build/PostSchedulerApp
```

### Automated Verification Mode
```bash
./build/PostSchedulerApp --test-run
```

## 9. Testing Instructions
Run CTest:
```bash
ctest --test-dir build --output-on-failure
```

## 10. Console Output Sample
```
========================================
   Automated Social Media Post Scheduler
  (SQLite Persistent Job Dispatch Engine)
========================================
Scheduling simulated publication jobs...
Queued Posts: ID #1, #2, #3
Waiting 1200ms for worker dispatch loop...

=== Scheduled Posts Database Ledger ===
  [PUBLISHED] ID #1 (Twitter):
     "Launching the modern C++ repository! #cpp #programming"
     Response: DISPATCH_ACK: Published to Twitter [ID: 1]
  [PUBLISHED] ID #2 (LinkedIn):
     "Excited to publish 60 advanced C++ applications."
     Response: DISPATCH_ACK: Published to LinkedIn [ID: 2]
  [PUBLISHED] ID #3 (Discord):
     "Dev server update live on port 8080."
     Response: DISPATCH_ACK: Published to Discord [ID: 3]
Scheduler stopped cleanly.
[Test Mode] Auto-post scheduler verified.
```

## 11. Known Limitations
- The default installation includes mock publishing adapters; connecting to live public APIs requires API keys/OAuth2 tokens passed via environment variables.

## 12. Testing Status
- **Built**: Yes, with GCC 16.1.0 on Windows linking SQLite3 with 0 warnings.
- **Run**: Verified scheduled job persistence, async background dispatch, and retry backoff with `--test-run`.
- **Tested**: All unit test suites passed under CTest.
