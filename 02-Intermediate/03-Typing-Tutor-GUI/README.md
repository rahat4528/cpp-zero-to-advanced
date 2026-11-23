# Project 03: Typing-Tutor-GUI

## 1. Project Overview
`Typing-Tutor-GUI` is an interactive typing instruction and analytics application designed to develop touch-typing fluency through structured curriculum tiers, real-time keystroke latency analysis, accuracy tracking, and mistake heatmaps.

## 2. Problem Statement
Learning touch-typing requires systematic drill practice across diverse character distributions (home-row keys, numbers, punctuation symbols, code syntaxes) paired with objective feedback on Words Per Minute (WPM) and specific keystroke error frequencies.

## 3. Objectives
- Offer progressive curriculum tiers from fundamental finger positioning to C++ code syntax.
- Deliver real-time per-keystroke accuracy and latency evaluation.
- Compute gross WPM, net WPM, and error percentages based on standardized 5-keystroke word normalization.
- Isolate the interactive presentation layer from the typing engine so it can run via TUI or be bound to Qt/Win32 GUI.

## 4. Features
- **Tiered Curriculum Catalog**: Home-row drills, common English bigrams, punctuation, and C++ idioms.
- **Precision Metrics**: Standard Gross WPM, Net WPM (penalized for errors), and Accuracy Percentage.
- **Mistake Distribution Matrix**: Tracks exactly which keys were missed and their frequencies.
- **Custom Lesson Registration**: Dynamically append user-defined typing passages.
- **Automated Validation Mode**: `--test-run` flag for headless CI/CD execution.

## 5. Technology Stack
- **Language**: C++17
- **Compiler**: GCC 11+, Clang 14+, MSVC 2022
- **Build System**: CMake 3.20+
- **Testing**: doctest 2.4+ via CTest
- **External Services**: None (100% offline)
- **Database**: In-memory curriculum catalog with extensibility for file/SQLite persistence.
- **GUI Toolkit Rationale**: Implemented with an interactive decoupled TUI/CLI engine to run universally without heavy external GUI framework dependencies while providing a clean separation of concerns for desktop GUI wrappers.

## 6. Folder Structure
```
03-Typing-Tutor-GUI/
├── include/
│   └── TypingTutor.hpp
├── src/
│   ├── TypingTutor.cpp
│   └── main.cpp
├── tests/
│   └── test_typing_tutor.cpp
├── CMakeLists.txt
├── README.md
└── .gitignore
```

## 7. Installation / Prerequisites
- CMake 3.20+
- C++17 compliant compiler (GCC, Clang, or MSVC)
- Ninja or Make

## 8. Build and Run Instructions
### Windows & Linux
```bash
cmake -S . -B build -G Ninja
cmake --build build
./build/TypingTutor
# Or run headless verification
./build/TypingTutor --test-run
```

## 9. Testing Instructions
```bash
ctest --test-dir build --output-on-failure
```

## 10. Console Output Sample
```
=================================================================
              INTERACTIVE TYPING TUTOR GUI / ENGINE               
=================================================================

AVAILABLE LESSON MODULES:
-----------------------------------------------------------------
 [1] Home Row Basics              Category: Fundamentals    Target: 25 WPM / 90%
 [2] Common English Bigrams       Category: Intermediate    Target: 35 WPM / 92%
 [3] Punctuation & Numeric Symbols Category: Intermediate    Target: 30 WPM / 90%
 [4] Modern C++ Idioms            Category: Advanced        Target: 40 WPM / 95%
-----------------------------------------------------------------

=================================================================
                      LESSON PERFORMANCE AUDIT                   
=================================================================
 Target: 25 WPM | 90% Accuracy
 Status: PASSED (COMPLETED)
-----------------------------------------------------------------
 Elapsed Time   : 4.82 s
 Total Keystrokes: 38 (38 correct, 0 errors)
 Accuracy       : 100.0 %
 Gross Speed    : 94.6 WPM
 Net Speed      : 94.6 WPM
=================================================================
```

## 11. Known Limitations
- The current interactive terminal interface reads by line in standard stdin mode; real-time per-character cursor coloring requires terminal raw mode (`termios` on POSIX or `SetConsoleMode` on Windows).

## 12. Testing Status
- **Built**: Yes (Clean compile with GCC 16.1.0 on Windows, 0 warnings).
- **Run**: Verified via interactive CLI and `--test-run` headless execution.
- **Tested**: 4/4 automated doctest unit test cases passing under CTest.
