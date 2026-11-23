# 07-Random-Quote-Generator

A modern C++17 Random Quote Retrieval and Filtering Utility powered by standard pseudo-random number generators.

---

## 1. Project Overview
Random Quote Generator ingests quote repositories from disk, provides uniform pseudo-random quote sampling, supports category and author filtering, and exposes deterministic seed configurations for testability.

## 2. Problem Statement
Developers frequently require inspirational, educational, or motivational quotes in shells, welcome banners, or applications. Implementing random generators properly requires avoiding legacy `rand()` in favor of modern `<random>` engines (`std::mt19937`) with uniform distributions.

## 3. Objectives
- Utilize `<random>` (`std::mt19937`, `std::uniform_int_distribution`, `std::random_device`).
- Parse pipe-delimited quote text files with comment support.
- Implement filtering by category and author substring.
- Support deterministic pseudo-random seeds (`--seed <uint>`) for reproducible testing.

## 4. Features
- **Mersenne Twister Engine**: High-entropy pseudo-randomness without modulo bias.
- **Category & Author Filtering**: Fast querying across tags (e.g. Technology, Inspiration, Wisdom).
- **Multiple Sampling**: Sample \(N\) non-repeating quotes with shuffle sampling.
- **Embedded Fallback**: Graceful fallback to built-in quotes if external files are missing.

## 5. Technology Stack
- **Language**: C++17 (`<random>`, `std::vector`, `std::set`, `std::fstream`, `std::string`)
- **Build System**: CMake 3.20+
- **Testing Framework**: doctest with CTest
- **Database Needed**: No. Plaintext file storage (`resources/quotes.txt`).
- **Internet / External Service Needed**: No.

## 6. Folder Structure
```
07-Random-Quote-Generator/
├── include/
│   └── QuoteGenerator.h       # Quote struct and QuoteGenerator API
├── src/
│   ├── QuoteGenerator.cpp     # File parsing, RNG engine, filtering
│   └── main.cpp              # CLI driver
├── resources/
│   └── quotes.txt            # Curated quote repository
├── tests/
│   └── test_quote_generator.cpp # doctest unit tests
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

# Print a random quote
.\build\RandomQuote.exe

# Filter by category
.\build\RandomQuote.exe --cat Technology

# Deterministic seed
.\build\RandomQuote.exe --seed 42
```

### Linux / macOS
```bash
cmake -S . -B build -G Ninja
cmake --build build

./build/RandomQuote --cat Technology
```

## 9. Testing Instructions
Run tests with CTest:
```bash
ctest --test-dir build --output-on-failure
```

## 10. Demo / Console Output Sample
```
$ ./build/RandomQuote --cat Technology

"There are only two kinds of languages: the ones people complain about and the ones nobody uses."
       -- Bjarne Stroustrup [Technology]
```

## 11. Known Limitations
- File parsing assumes single-line quotes.
- No network quote API integration (intentionally offline for beginner tier).

## 12. Testing Status
- **Built**: Yes, compiled cleanly with GCC 16.1.0 (`-Wall -Wextra -Wpedantic`).
- **Run**: Yes, verified random selection, seed reproducibility, and category filtering.
- **Tested**: Yes, 4 test suites passing via CTest.
- **Hardware/Credentials Limitations**: None.
