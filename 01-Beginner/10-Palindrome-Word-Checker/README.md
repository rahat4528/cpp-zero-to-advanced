# 10-Palindrome-Word-Checker

A high-performance modern C++17 string processing utility for palindrome detection and longest palindromic substring discovery.

---

## 1. Project Overview
Palindrome Word Checker inspects single words, sentences, and numerical strings to detect symmetry. It uses a zero-allocation two-pointer algorithm with `std::string_view`, handles complex punctuation/case normalization, and discovers the longest palindromic substring using center-expansion.

## 2. Problem Statement
String symmetry verification must handle non-alphanumeric punctuation marks, casing differences, whitespace variations, and large text streams without redundant heap allocations or performance regressions.

## 3. Objectives
- Utilize `std::string_view` for zero-copy string inspection.
- Implement an \(O(N)\) time and \(O(1)\) space two-pointer verification algorithm.
- Provide both normalized (ignoring whitespace and punctuation) and strict character-exact matching modes.
- Implement center-expansion search for longest palindromic substrings.

## 4. Features
- **Zero-Copy Performance**: Operates directly on string views.
- **Phrase Detection**: Correctly identifies classic palindromes like "A man, a plan, a canal: Panama!".
- **Longest Palindromic Substring**: Identifies maximal palindromes within arbitrary texts.
- **Dual Matching Modes**: Configurable via `--strict` flag.

## 5. Technology Stack
- **Language**: C++17 (`std::string_view`, `std::string`, `std::vector`, `<cctype>`)
- **Build System**: CMake 3.20+
- **Testing Framework**: doctest with CTest
- **Database Needed**: No.
- **Internet / External Service Needed**: No.

## 6. Folder Structure
```
10-Palindrome-Word-Checker/
├── include/
│   └── PalindromeChecker.h        # Two-pointer API and declarations
├── src/
│   ├── PalindromeChecker.cpp      # Normalization and verification algorithms
│   └── main.cpp                   # CLI driver
├── tests/
│   └── test_palindrome_checker.cpp# doctest unit tests
├── CMakeLists.txt                 # CMake build configuration
├── .gitignore                     # Build ignore rules
└── README.md                      # Project documentation
```

## 7. Installation / Prerequisites
- GCC 11+, Clang 14+, or MSVC 2022
- CMake 3.20+ and Ninja (or Make)

## 8. Build and Run Instructions

### Windows (PowerShell)
```powershell
cmake -S . -B build -G Ninja
cmake --build build

# Normalized phrase check
.\build\PalindromeChecker.exe --text "A man, a plan, a canal: Panama!"

# Strict check
.\build\PalindromeChecker.exe --strict --text "racecar"

# Find longest palindromic substring
.\build\PalindromeChecker.exe --longest --text "abacdfgdcaba"
```

### Linux / macOS
```bash
cmake -S . -B build -G Ninja
cmake --build build

./build/PalindromeChecker --text "racecar"
```

## 9. Testing Instructions
Run tests with CTest:
```bash
ctest --test-dir build --output-on-failure
```

## 10. Demo / Console Output Sample
```
$ ./build/PalindromeChecker --text "A man, a plan, a canal: Panama!"
Text       : "A man, a plan, a canal: Panama!"
Mode       : Normalized (ignoring punctuation & case)
Verdict    : PALINDROME (YES)
```

## 11. Known Limitations
- Unicode multi-byte UTF-8 grapheme clusters (e.g. accented characters) are normalized at the ASCII boundary.
- Longest substring search operates in \(O(N^2)\) time via center expansion (sufficient for input strings < 10,000 characters).

## 12. Testing Status
- **Built**: Yes, compiled with GCC 16.1.0 using `-Wall -Wextra -Wpedantic`.
- **Run**: Yes, verified both normalized and strict matching.
- **Tested**: Yes, 6 comprehensive test suites passing via CTest.
- **Hardware/Credentials Limitations**: None.
