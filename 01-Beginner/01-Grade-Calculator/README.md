# 01-Grade-Calculator

A modern C++17 grade calculator, report generator, and GPA calculation engine for students and academic institutions.

## 1. Project Overview
The **Grade Calculator** CLI application processes student subject marks, computes individual percentage scores, assigns letter grades and grade points based on a standard grade table, calculates credit-weighted Grade Point Averages (GPA), and determines overall pass/fail status with fail-on-sub-40% subject safeguards.

## 2. Problem Statement
Manual grade calculations and academic report card creation are prone to human calculation errors, especially when computing credit-weighted GPAs, applying multi-tiered letter grade thresholds, and enforcing strict academic progression rules (such as failing the entire term if any single subject falls below minimum competence, regardless of overall average).

## 3. Objectives
- Support 1 to 20 subject entries with custom titles, marks obtained, custom maximum marks, and credit weightings.
- Enforce the standard academic grade scale:
  - 80%+ : `A+` (4.00)
  - 75%+ : `A`  (3.75)
  - 70%+ : `A-` (3.50)
  - 65%+ : `B+` (3.25)
  - 60%+ : `B`  (3.00)
  - 55%+ : `B-` (2.75)
  - 50%+ : `C+` (2.50)
  - 45%+ : `C`  (2.25)
  - 40%+ : `D`  (2.00)
  - < 40%: `F`  (0.00)
- Calculate overall total marks, overall percentage, and credit-weighted GPA ($\sum (GP_i \times C_i) / \sum C_i$).
- Enforce strict fail criteria: Any subject with $< 40\%$ immediately results in an overall `FAIL`.
- Output an aligned tabular terminal report and optionally persist it to `report_<name>.txt`.
- Robust input stream re-prompting on malformed non-numeric inputs.

## 4. Features
- **Credit-Weighted GPA Calculation**: Accurately weights course points against credit units.
- **Fail-Fast Subject Protection**: Automatically detects deficiencies in single subjects.
- **Configurable Grade Table**: Decoupled grade thresholds encapsulated in `GradeTable`.
- **Pure Functional Core**: All calculations (`percentage`, `letter_for`, `gpa`, `overall_result`) are deterministic pure functions isolated from I/O.
- **File Persistence**: Exports clean ASCII tabular reports to `report_<student_name>.txt`.
- **Defensive Input Handling**: Clears `std::cin` failbits and discards line buffers on invalid input.

## 5. Technology Stack
- **Language**: C++17 (`std::vector`, `std::pair`, structured bindings).
- **Libraries**: C++ Standard Library (`<iostream>`, `<iomanip>`, `<sstream>`, `<fstream>`).
- **Build System**: CMake 3.20+.
- **Testing Framework**: doctest.
- **Database / Network**: None required (100% self-contained local CLI).

## 6. Folder Structure
```
01-Grade-Calculator/
├── CMakeLists.txt
├── .gitignore
├── README.md
├── include/
│   └── GradeCalculator.h
├── src/
│   ├── GradeCalculator.cpp
│   └── main.cpp
└── tests/
    └── test_grade_calculator.cpp
```

## 7. Prerequisites & Installation
- **C++ Compiler**: GCC 11+, Clang 14+, or MSVC 2022 supporting C++17.
- **CMake**: Version 3.20 or newer.
- **Ninja / Make**: Build automation tool.

## 8. Build and Run Instructions

### Build
```powershell
cmake -S . -B build -G Ninja
cmake --build build
```

### Run
```powershell
.\build\GradeCalculator.exe
# Non-interactive automated smoke run:
.\build\GradeCalculator.exe --test-run
```

## 9. Testing Instructions
Run unit tests with CTest:
```powershell
ctest --test-dir build --output-on-failure
```

Direct unit test binary execution:
```powershell
.\build\GradeCalculator_tests.exe
```

## 10. Console Output Demo
```text
=========================================================================
                        STUDENT GRADE REPORT                             
=========================================================================
Student Name: Alex Mercer
-------------------------------------------------------------------------
Subject                    Marks       Max    Credit    Pct(%)   Grade   Point
-------------------------------------------------------------------------
Mathematics                 88.0     100.0       4.0     88.00      A+    4.00
Physics                     74.0     100.0       3.0     74.00      A-    3.50
Computer Science            95.0     100.0       4.0     95.00      A+    4.00
-------------------------------------------------------------------------
Total Marks Obtained : 257.00 / 300.00
Overall Percentage   : 85.67%
Weighted GPA         : 3.86 / 4.00
Final Result         : PASS
=========================================================================
```

## 11. Known Limitations
- Terminal output is formatted for fixed 80-column ASCII displays.
- Letter grade definitions are pre-configured to the 10-tier university scale by default.

## 12. Testing Status
- **Built**: Yes, compiled cleanly with GCC 16.1.0 with `-Wall -Wextra -Wpedantic` (0 warnings).
- **Run**: Verified interactive flow and non-interactive `--test-run`.
- **Tested**: All 7 comprehensive doctest test cases passed (boundary values 39.99, 40, 59.99, 60; extremes 0% and 100%; custom max 50; single subject; all-fail rule; hand-computed weighted GPA; and input validation rejections).
