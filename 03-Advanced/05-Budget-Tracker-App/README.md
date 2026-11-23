# 05-Budget-Tracker-App

## 1. Project Overview
05-Budget-Tracker-App is an advanced personal finance and budget management console application. It combines an embedded relational SQLite database, monthly spending quotas, budget threshold tracking, cash flow analytics, and dynamic horizontal ASCII bar charts to visualize spending categories.

## 2. Problem Statement
Personal budget tracking requires strict data integrity, fast querying across date ranges, and actionable visual summaries. Spreadsheet solutions often lack relational schema validation, while heavy cloud apps require proprietary subscriptions. This project delivers a standalone, ACID-compliant financial tracker that operates locally without external data leaks.

## 3. Objectives
- Implement relational SQLite3 data models for income and expense categories, monthly budgets, and timestamped transactions.
- Calculate key financial indicators: Total Income, Total Expenses, Net Cash Flow, Savings Rate (%), and Category Budget Adherence.
- Provide ASCII horizontal bar charts for clear terminal data visualization without requiring external graphical display servers.
- Supply an automated smoke test mode (`--test-run`) and full unit test coverage via doctest.

## 4. Features
- **ACID Relational Storage**: Schema enforced by SQLite with foreign key relationships between transactions and categories.
- **Budget Threshold Alerts**: Real-time detection when category spending exceeds allocated monthly quotas.
- **Monthly Financial Statements**: Tabular summary report detailing earnings, outlays, and net cash flow.
- **Terminal Bar Visualizations**: Scaled horizontal ASCII charts mapping category expenditures proportionally.
- **In-Memory & File Storage**: Supports `:memory:` databases for rapid testing and persistent `.db` files for production usage.

## 5. Technology Stack
- **Language**: C++17 (`std::vector`, `std::string`, `std::optional`, `std::iomanip`, `std::ostringstream`).
- **Database**: SQLite3 via C API (`sqlite3_prepare_v2`, `sqlite3_bind_*`, `sqlite3_step`).
- **External Services**: None. 100% local and offline.
- **Testing**: `doctest` unit testing framework.

## 6. Folder Structure
```
05-Budget-Tracker-App/
├── CMakeLists.txt
├── README.md
├── .gitignore
├── include/
│   ├── BudgetDatabase.hpp
│   └── ReportGenerator.hpp
├── src/
│   ├── BudgetDatabase.cpp
│   ├── ReportGenerator.cpp
│   └── main.cpp
└── tests/
    └── test_budget_tracker.cpp
```

## 7. Installation / Prerequisites
- CMake 3.20 or newer.
- GCC 11+, Clang 14+, or MSVC 2022.
- SQLite3 development library (`libsqlite3`).
- doctest headers.

## 8. Build and Run Instructions
### Windows & Linux Build
```bash
cmake -S . -B build -G Ninja
cmake --build build
```

### Running the Application
```bash
./build/BudgetTracker
```

### Automated Verification Mode
```bash
./build/BudgetTracker --test-run
```

## 9. Testing Instructions
Execute the unit test suite via CTest:
```bash
ctest --test-dir build --output-on-failure
```

## 10. Console Output Sample
```
========================================
        FINANCIAL SUMMARY REPORT        
             Period: 2026-11
========================================
 Total Income:       $4500.00
 Total Expense:      $2245.00
 Net Cash Flow:      $2255.00
 Savings Rate:        50.11%
----------------------------------------
 Budget Adherence:
  - Housing        Spent: $1450.00  / Budget: $1500.00  (96.7%)
  - Food & Dining  Spent: $400.00   / Budget: $600.00   (66.7%)
  - Utilities      Spent: $165.00   / Budget: $250.00   (66.0%)
  - Entertainment  Spent: $230.00   / Budget: $200.00   (115.0%) [EXCEEDED!]

=== Spending Distribution Chart ===
Housing         | ############################## $1450.00
Food & Dining   | ########                       $400.00
Utilities       | ###                            $165.00
Entertainment   | ####                           $230.00
====================================
```

## 11. Known Limitations
- Transaction dates must follow the ISO-8601 `YYYY-MM-DD` string format.
- Multi-currency conversions are not supported; all amounts assume a single base currency unit.

## 12. Testing Status
- **Built**: Yes, with GCC 16.1.0 on Windows with 0 warnings (`-Wall -Wextra -Wpedantic`).
- **Run**: Verified with `--test-run` and live sample generation.
- **Tested**: All 3 test suites passed under CTest.
