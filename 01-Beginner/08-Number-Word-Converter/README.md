# 08-Number-Word-Converter

A modern C++17 application converting 64-bit numerical values into formal English word representations and financial currency strings.

---

## 1. Project Overview
Number Word Converter translates numerical values (from 0 up to quadrillions, including negative integers and financial currency decimals) into grammatical English phrases.

## 2. Problem Statement
Check printing, legal contracts, and financial receipts require numerical figures to be spelled out in words to eliminate ambiguity and prevent tampering. Translating numbers correctly requires recursive chunking into thousands periods and hyphenating compound numbers.

## 3. Objectives
- Implement recursive 3-digit chunk parsing (Hundreds, Tens, Units).
- Correctly format scale words (Thousand, Million, Billion, Trillion).
- Handle negative integers with proper prefixes.
- Provide monetary check-spelling mode with Dollars and Cents formatting.

## 4. Features
- **64-Bit Range**: Converts numbers from \(-10^{15}\) to \(+10^{15}\).
- **Hyphenation Rules**: Accurate hyphenation for numbers between 21 and 99 (e.g. "Twenty-One").
- **Currency Mode**: Formats dollar and cent phrases with pluralization ("Dollar" vs "Dollars", "Cent" vs "Cents").
- **Scriptable CLI**: Automated execution via `--num` and `--currency`.

## 5. Technology Stack
- **Language**: C++17 (`std::vector`, `std::array`, `std::ostringstream`, `int64_t`)
- **Build System**: CMake 3.20+
- **Testing Framework**: doctest with CTest
- **Database Needed**: No. In-memory string translation algorithms.
- **Internet / External Service Needed**: No.

## 6. Folder Structure
```
08-Number-Word-Converter/
├── include/
│   └── NumberWordConverter.h       # Function declarations
├── src/
│   ├── NumberWordConverter.cpp     # Number to word translation algorithms
│   └── main.cpp                    # CLI driver
├── tests/
│   └── test_number_word_converter.cpp # doctest unit test suite
├── CMakeLists.txt                  # CMake build configuration
├── .gitignore                      # Build ignore rules
└── README.md                       # Project documentation
```

## 7. Installation / Prerequisites
- GCC 11+, Clang 14+, or MSVC 2022
- CMake 3.20+ and Ninja (or Make)

## 8. Build and Run Instructions

### Windows (PowerShell)
```powershell
cmake -S . -B build -G Ninja
cmake --build build

# Convert integer
.\build\NumberWordConverter.exe --num 1042

# Convert currency amount
.\build\NumberWordConverter.exe --currency 2540.75
```

### Linux / macOS
```bash
cmake -S . -B build -G Ninja
cmake --build build

./build/NumberWordConverter --num 1042
```

## 9. Testing Instructions
Run tests with CTest:
```bash
ctest --test-dir build --output-on-failure
```

## 10. Demo / Console Output Sample
```
$ ./build/NumberWordConverter --num 123456789
123456789 = One Hundred Twenty-Three Million Four Hundred Fifty-Six Thousand Seven Hundred Eighty-Nine

$ ./build/NumberWordConverter --currency 42.50
$42.5 = Forty-Two Dollars and Fifty Cents
```

## 11. Known Limitations
- English language rules only (UK/US variant differences: does not include "and" between hundreds and tens by default).
- Floating point precision beyond 2 decimal places is rounded to nearest cents.

## 12. Testing Status
- **Built**: Yes, compiled with GCC 16.1.0 using `-Wall -Wextra -Wpedantic`.
- **Run**: Yes, verified both integer and currency flags.
- **Tested**: Yes, 6 comprehensive test suites passing via CTest.
- **Hardware/Credentials Limitations**: None.
