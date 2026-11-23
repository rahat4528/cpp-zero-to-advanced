# 11-Quiz-Game-UI

An interactive Quiz Game and Examination Terminal UI application with structured question bank loading built with modern C++17.

---

## 1. Project Overview
Quiz Game UI is an educational trivia and examination runner. It loads structured multiple-choice questions from disk files, guides users through question sessions, scores answers dynamically, and presents detailed grade evaluations.

## 2. Problem Statement
Educational testing utilities need to decouple question data definitions from UI rendering logic, support custom question bank authoring without recompilation, validate user answer entries, and compute metrics accurately.

## 3. Objectives
- Parse multi-line question blocks with questions, multiple-choice options, correct keys, and explanations.
- Manage quiz state, scores, answer validations, and percentages.
- Deliver a clean, structured Terminal User Interface (TUI).
- Support automated test execution via `--answers` command-line argument.

## 4. Features
- **File-Driven Bank**: Easily author questions in `resources/questions.txt` without editing source code.
- **Immediate Feedback**: Explains why an answer was correct or incorrect after every question.
- **Grading Engine**: Computes points, accuracy percentages, and standard letter grade evaluations.
- **Headless Testing**: `--answers "B,C,B,C"` flag enables non-interactive automated test verification.

## 5. Technology Stack
- **Language**: C++17 (`std::vector`, `std::fstream`, `std::optional`, `std::string`)
- **UI Toolkit**: Interactive Terminal UI (TUI). *Rationale*: Selected to avoid bulky 5GB Qt6 SDK dependencies while providing a clean, cross-platform interactive quiz experience that builds and runs instantly.
- **Build System**: CMake 3.20+
- **Testing Framework**: doctest with CTest
- **Database Needed**: No. Plaintext question bank file.
- **Internet / External Service Needed**: No.

## 6. Folder Structure
```
11-Quiz-Game-UI/
├── include/
│   └── QuizGame.h             # Question struct and QuizEngine API
├── src/
│   ├── QuizGame.cpp           # File parsing and scoring logic
│   └── main.cpp              # Terminal UI and game loop
├── resources/
│   └── questions.txt          # Default question bank
├── tests/
│   └── test_quiz_game.cpp     # doctest unit test suite
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

# Interactive game
.\build\QuizGame.exe

# Automated test run
.\build\QuizGame.exe --answers "B,C,B,C"
```

### Linux / macOS
```bash
cmake -S . -B build -G Ninja
cmake --build build

./build/QuizGame --answers "B,C,B,C"
```

## 9. Testing Instructions
Run tests with CTest:
```bash
ctest --test-dir build --output-on-failure
```

## 10. Demo / Console Output Sample
```
$ ./build/QuizGame --answers "B,C,B,C"
Question 1 Answer: B -> CORRECT
Question 2 Answer: C -> CORRECT
Question 3 Answer: B -> CORRECT
Question 4 Answer: C -> CORRECT

Quiz Score: 40/40 (100%) Grade: A (Outstanding)
```

## 11. Known Limitations
- Terminal UI mode; graphical widget frontend is planned in Intermediate tier (`02-Intermediate/15-Quiz-Timer-App`).
- Questions currently support single-select answers (A/B/C/D).

## 12. Testing Status
- **Built**: Yes, compiled with GCC 16.1.0 using `-Wall -Wextra -Wpedantic`.
- **Run**: Yes, tested both `--answers` automated run and interactive modes.
- **Tested**: Yes, 3 test suites passing via CTest.
- **Hardware/Credentials Limitations**: None.
