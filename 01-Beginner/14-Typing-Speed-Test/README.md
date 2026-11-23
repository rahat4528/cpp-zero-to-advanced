# 14-Typing-Speed-Test

A Words-Per-Minute (WPM), accuracy, and Levenshtein edit distance typing benchmark application built with modern C++17.

---

## 1. Project Overview
Typing Speed Test measures human keyboard performance. It calculates elapsed duration using high-resolution monotonic clocks (`<chrono>`), counts keystrokes, calculates exact edit distance errors using dynamic programming Levenshtein algorithms, and computes both Raw WPM and Net WPM.

## 2. Problem Statement
Accurately benchmarking typing velocity requires standard word length normalization (5 characters = 1 standard word), uncorrected error penalization (Net WPM), and string edit distance comparisons to detect skipped, added, or substituted letters.

## 3. Objectives
- Utilize high-resolution `std::chrono::steady_clock` timers.
- Implement the standard Levenshtein distance algorithm using dynamic programming.
- Calculate Raw WPM and Net WPM according to international typing standards.
- Support automated test execution via `--prompt`, `--input`, and `--time`.

## 4. Features
- **Standardized WPM Metrics**: Standard 5-character word normalization.
- **Levenshtein Distance**: Measures insertions, deletions, and substitutions accurately.
- **Net Speed Penalties**: Discourages reckless typing by subtracting uncorrected errors from raw velocity.
- **Skill Categorization**: Classifies typist capability from Novice to Expert.

## 5. Technology Stack
- **Language**: C++17 (`<chrono>`, `std::vector`, `std::string`, `<algorithm>`)
- **Build System**: CMake 3.20+
- **Testing Framework**: doctest with CTest
- **Database Needed**: No.
- **Internet / External Service Needed**: No.

## 6. Folder Structure
```
14-Typing-Speed-Test/
├── include/
│   └── TypingSpeedTest.h        # Evaluation structures and API
├── src/
│   ├── TypingSpeedTest.cpp      # Levenshtein distance & WPM arithmetic
│   └── main.cpp                 # CLI driver and interactive loop
├── tests/
│   └── test_typing_speed.cpp    # doctest unit test suite
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

# Interactive test
.\build\TypingSpeedTest.exe

# Automated test execution
.\build\TypingSpeedTest.exe --prompt "The quick brown fox jumps over the lazy dog." --input "The quick brown fox jumps over the lazy dog." --time 15.0
```

### Linux / macOS
```bash
cmake -S . -B build -G Ninja
cmake --build build

./build/TypingSpeedTest --prompt "Hello World" --input "Hello World" --time 2.0
```

## 9. Testing Instructions
Run tests with CTest:
```bash
ctest --test-dir build --output-on-failure
```

## 10. Demo / Console Output Sample
```
$ ./build/TypingSpeedTest --prompt "The quick brown fox jumps over the lazy dog." --input "The quick brown fox jumps over the lazy dog." --time 15.0

============================================
             TYPING TEST RESULTS            
============================================
Elapsed Time   : 15.0 seconds
Characters     : 44 typed (44 exact matches)
Edit Errors    : 0 (Levenshtein distance)
Accuracy       : 100.0 %
--------------------------------------------
Raw Speed      : 35.2 WPM
Net Speed      : 35.2 WPM (Net Words Per Minute)
Rating Level   : Beginner
============================================
```

## 11. Known Limitations
- Terminal line buffering mode requires pressing Enter to submit the prompt.
- GUI tutor mode is featured in Intermediate tier (`02-Intermediate/03-Typing-Tutor-GUI`).

## 12. Testing Status
- **Built**: Yes, compiled with GCC 16.1.0 using `-Wall -Wextra -Wpedantic`.
- **Run**: Yes, verified both interactive and automated CLI runs.
- **Tested**: Yes, 4 unit test suites passing via CTest.
- **Hardware/Credentials Limitations**: None.
