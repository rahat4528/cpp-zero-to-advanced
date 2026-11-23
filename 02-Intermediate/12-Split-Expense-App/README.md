# Project 12: Split-Expense-App

## 1. Project Overview
`Split-Expense-App` is an object-oriented expense sharing and debt settlement manager. It models group expenditure, computes member ledger balances, and applies a directed graph greedy flow algorithm to compute the minimal set of monetary transactions required to settle all debts.

## 2. Problem Statement
Group travel and collaborative projects often lead to entangled multi-party debts where individual pairwise settlements result in redundant cash transfers. A cash-flow minimization algorithm is needed to reduce transactional overhead.

## 3. Objectives
- Design clean object-oriented domain models for group participants, expense line items, and settlement transfers.
- Compute exact individual ledger balances across asymmetric group divisions.
- Implement a greedy bipartite matching algorithm with dual max-heaps (debtors and creditors) minimizing total transaction count.
- Serialize and restore group expenses to persistent disk storage.

## 4. Features
- **Cash Flow Simplification**: Converts tangled $O(N^2)$ pairwise debts into a minimized set of at most $N-1$ settlement transfers.
- **Asymmetric Split Support**: Dynamically assign any subset of members to participate in an individual expense.
- **Roundtrip Persistence**: Formatted plain-text storage retaining member IDs, names, and transaction breakdowns.
- **Comprehensive Unit Testing**: Verifies net ledger balances, debt bypass routing, and serialization fidelity.

## 5. Technology Stack
- **Language**: C++17
- **Compiler**: GCC 11+, Clang 14+, MSVC 2022
- **Build System**: CMake 3.20+
- **Containers**: Standard STL (`std::priority_queue`, `std::map`, `std::vector`)
- **Testing**: doctest 2.4+ via CTest
- **External Services**: None (100% offline)

## 6. Folder Structure
```
12-Split-Expense-App/
├── include/
│   └── ExpenseManager.hpp
├── src/
│   ├── ExpenseManager.cpp
│   └── main.cpp
├── tests/
│   └── test_split_expense.cpp
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

# Interactive mode:
./build/SplitExpenseApp

# Headless scenario verification:
./build/SplitExpenseApp --test-run
```

## 9. Testing Instructions
```bash
ctest --test-dir build --output-on-failure
```

## 10. Console Output Sample
```
=================================================================
                 SPLIT EXPENSE APPLICATION (OOP)                 
=================================================================
[Test-Run Mode] Simulating group trip expenses and debt settlement...
Calculated Net Balances:
  Alice     : +$80.00
  Bob       : -$10.00
  Charlie   : -$30.00
  Diana     : -$40.00

Optimized Cash Transfers (Minimal Settlement):
  Diana pays Alice $40.00
  Charlie pays Alice $30.00
  Bob pays Alice $10.00
```

## 11. Known Limitations
- Does not currently support currency conversion between differing currencies (assumes single currency denomination).

## 12. Testing Status
- **Built**: Yes (Clean compilation under GCC 16.1.0 on Windows, 0 warnings).
- **Run**: Verified `--test-run` and interactive console mode.
- **Tested**: 2 multi-subcase test suites passing under CTest.
