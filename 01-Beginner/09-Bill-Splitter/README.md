# 09-Bill-Splitter

A modern C++17 Bill Splitter and Expense Sharing utility with tax, tip calculations, and penny-exact remainder distribution algorithms.

---

## 1. Project Overview
Bill Splitter provides group meal and service bill calculation. It computes sales tax, customizable tip percentages, and partitions the final grand total among \(N\) participants either equally or item-by-item, guaranteeing that rounded penny fractions sum up exactly to the total check.

## 2. Problem Statement
Dividing bills frequently introduces floating-point rounding discrepancies where individual rounded contributions do not equal the bill charged to the credit card. Furthermore, friends frequently consume different menu items requiring proportional itemized calculations.

## 3. Objectives
- Compute subtotal, sales tax, and tip percentages accurately in integer cents to prevent floating-point drift.
- Implement an algorithm distributing fractional penny remainders across diners fairly.
- Support both equal splits and proportional itemized splits.
- Defensively validate invalid input (zero diners, negative amounts, out-of-range tax).

## 4. Features
- **Penny-Exact Remainder Algorithm**: Eliminates lost pennies during division.
- **Itemized Multi-Diner Sharing**: Calculates individual obligations when dishes are shared by subsets of friends.
- **Custom Tax & Tip Rates**: Flexible percentages supported.
- **Scriptable CLI**: Automated flags for integration testing (`--total`, `--people`, `--tip`, `--tax`, `--itemized`).

## 5. Technology Stack
- **Language**: C++17 (`std::vector`, `std::map`, `std::optional`, `std::string`, `int64_t`)
- **Build System**: CMake 3.20+
- **Testing Framework**: doctest with CTest
- **Database Needed**: No.
- **Internet / External Service Needed**: No.

## 6. Folder Structure
```
09-Bill-Splitter/
├── include/
│   └── BillSplitter.h         # API for equal and itemized splits
├── src/
│   ├── BillSplitter.cpp       # Penny-exact arithmetic algorithms
│   └── main.cpp              # CLI driver and report formatter
├── tests/
│   └── test_bill_splitter.cpp # doctest unit tests
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

# Equal split with 3 people
.\build\BillSplitter.exe --total 100.00 --people 3 --tip 20 --tax 8.875

# Itemized split
.\build\BillSplitter.exe --itemized "Burger:15:Alice;Pasta:18:Bob;Dessert:10:Alice,Bob" --tip 18
```

### Linux / macOS
```bash
cmake -S . -B build -G Ninja
cmake --build build

./build/BillSplitter --total 100.00 --people 3
```

## 9. Testing Instructions
Run tests with CTest:
```bash
ctest --test-dir build --output-on-failure
```

## 10. Demo / Console Output Sample
```
$ ./build/BillSplitter --total 100.00 --people 3 --tip 20 --tax 8.875

========================================
              BILL SUMMARY              
========================================
Subtotal     : $100.00
Tax (8.88%): $8.88
Tip (20.00%): $20.00
Grand Total  : $128.88
People Count : 3
----------------------------------------
Base Per Person : $42.96
Exact Distribution (Penny-Exact):
  Person 1: $42.96
  Person 2: $42.96
  Person 3: $42.96
========================================
```

## 11. Known Limitations
- Itemized parser uses simple colon/semicolon format.
- Currency formatting assumes USD ($) symbols.

## 12. Testing Status
- **Built**: Yes, compiled cleanly with GCC 16.1.0 (`-Wall -Wextra -Wpedantic`).
- **Run**: Yes, tested equal and itemized splits with penny remainder distributions.
- **Tested**: Yes, 5 unit test suites passing via CTest.
- **Hardware/Credentials Limitations**: None.
