# Project 15: Quiz-Timer-App

## 1. Project Overview
`Quiz-Timer-App` is a time-pressured technical knowledge testing application with dynamic countdown timing, speed bonus calculations, timeout forfeits, and comprehensive performance analytics.

## 2. Problem Statement
Examinations require precise timing mechanisms where score rewards reflect not merely factual accuracy, but the speed and confidence of answers under time pressure.

## 3. Objectives
- Implement an evaluation engine tracking high-resolution response latencies.
- Calculate non-linear point allocations combining base difficulty points with fractional speed bonuses.
- Automatically disqualify answers submitted after question timeout limits.
- Deliver aggregate summary metrics (accuracy percentage, timeout rates, average response latency).

## 4. Features
- **Dynamic Speed Bonus**: Correct answers awarded up to 50% additional bonus points proportional to remaining time ($t_{remain} / t_{limit}$).
- **Timeout Protection**: Questions submitted past expiration receive zero credit and are flagged as timed-out in the ledger.
- **Detailed Audit Trail**: Logs per-question response times, chosen options, correctness, and final point allocations.
- **Headless Test Support**: `--test-run` flag allows non-interactive verification across automated CI workflows.

## 5. Technology Stack
- **Language**: C++17
- **Compiler**: GCC 11+, Clang 14+, MSVC 2022
- **Build System**: CMake 3.20+
- **Timing**: Standard C++ `<chrono>`
- **Testing**: doctest 2.4+ via CTest
- **GUI Toolkit Rationale**: Implemented with clean decoupling between the timer/scoring engine and presentation layer, enabling both terminal and Qt GUI frontends.

## 6. Folder Structure
```
15-Quiz-Timer-App/
├── include/
│   └── QuizTimerEngine.hpp
├── src/
│   ├── QuizTimerEngine.cpp
│   └── main.cpp
├── tests/
│   └── test_quiz_timer.cpp
├── CMakeLists.txt
├── README.md
└── .gitignore
```

## 7. Installation / Prerequisites
- CMake 3.20+
- C++17 compliant compiler
- Ninja or Make

## 8. Build and Run Instructions
### Windows & Linux
```bash
cmake -S . -B build -G Ninja
cmake --build build

# Headless scenario verification:
./build/QuizTimerApp --test-run
```

## 9. Testing Instructions
```bash
ctest --test-dir build --output-on-failure
```

## 10. Console Output Sample
```
=================================================================
                TIMED TECHNICAL QUIZ ENGINE (TUI)                
=================================================================
[Test-Run Mode] Simulating timed answering sequence...

QUIZ PERFORMANCE AUDIT:
  - Total Questions  : 4
  - Correct Answers  : 2
  - Timed Out        : 1
  - Total Score Earned: 261 / 600 (43.5%)
  - Avg Response Time : 5.175 s
```

## 11. Known Limitations
- Rich visual graphical countdown animations require integration with a graphics library (e.g. Qt Quick / QML).

## 12. Testing Status
- **Built**: Yes (Clean compilation under GCC 16.1.0 on Windows, 0 warnings).
- **Run**: Verified `--test-run` and scoring calculation.
- **Tested**: 2 multi-subcase test suites passing under CTest.
