# Project 05: Inventory-Manager-UI

## 1. Project Overview
`Inventory-Manager-UI` is an enterprise warehouse stock tracking and auditing application powered by an embedded SQLite relational database engine. It tracks inventory units, SKUs, reorder thresholds, suppliers, and transactional restock/dispatch histories.

## 2. Problem Statement
Manual inventory logs suffer from human counting errors, missed reorder deadlines, and untracked stock shrinkages. A structured database with automated valuation calculations and low-stock alerts is required to ensure warehouse supply continuity.

## 3. Objectives
- Construct an ACID-compliant schema with foreign key constraints and transactional integrity for stock level modifications.
- Deliver real-time valuation reports and automatic warning alerts when stock quantities fall below threshold limits.
- Log immutable audit trails for every inventory movement (restock, dispatch, adjustments).
- Maintain an architecture where presentation is cleanly separated from the underlying SQLite storage engine.

## 4. Features
- **ACID Transactions**: Atomic updates linking product quantities and transaction history.
- **Low-Stock Detection**: Query engine surfacing SKUs requiring replenishment.
- **Valuation Analytics**: Computes aggregate unit counts and current inventory market valuation.
- **Over-Dispatch Prevention**: Rejects any transaction that would result in negative quantity on hand.
- **In-Memory Testing Support**: Unit tests execute on `:memory:` SQLite instances for fast, isolated CI tests.

## 5. Technology Stack
- **Language**: C++17
- **Compiler**: GCC 11+, Clang 14+, MSVC 2022
- **Build System**: CMake 3.20+
- **Database**: SQLite3 via C API (`SQLite3::SQLite3`)
- **Testing**: doctest 2.4+ via CTest
- **GUI Toolkit Rationale**: Implemented via a high-density tabular terminal dashboard; separation of core engine from presentation allows drop-in binding to Qt/QML or WinUI.

## 6. Folder Structure
```
05-Inventory-Manager-UI/
├── include/
│   └── InventoryManager.hpp
├── src/
│   ├── InventoryManager.cpp
│   └── main.cpp
├── tests/
│   └── test_inventory.cpp
├── CMakeLists.txt
├── vcpkg.json
├── README.md
└── .gitignore
```

## 7. Installation / Prerequisites
- CMake 3.20+
- C++17 compliant compiler
- SQLite3 development headers and library

## 8. Build and Run Instructions
### Windows & Linux
```bash
cmake -S . -B build -G Ninja
cmake --build build

# Run with local SQLite database:
./build/InventoryManager

# Run automated headless verification:
./build/InventoryManager --test-run
```

## 9. Testing Instructions
```bash
ctest --test-dir build --output-on-failure
```

## 10. Console Output Sample
```
=================================================================
              WAREHOUSE INVENTORY MANAGER (SQLite)               
=================================================================
Distinct Products: 3
Total Units      : 20
Total Valuation  : $735.50
Low Stock Alerts : 2

====================================================================================================
SKU         PRODUCT NAME             CATEGORY          PRICE ($)   STOCK     REORDER   SUPPLIER       
----------------------------------------------------------------------------------------------------
SKU-1003    USB-C PD Cable 2m        Accessories       8.50        2         8         Anker          
SKU-1002    Microcontroller Board    Electronics       12.00       3         10        Adafruit       
====================================================================================================
```

## 11. Known Limitations
- Multi-user remote access requires a client-server architecture or SQLite WAL mode with concurrent file lock handling.

## 12. Testing Status
- **Built**: Yes (Compiled cleanly with GCC 16.1.0 on Windows, 0 warnings).
- **Run**: Verified interactive dashboard and `--test-run` headless mode.
- **Tested**: 3/3 comprehensive test cases (CRUD, stock adjustments, valuation) passing under CTest.
