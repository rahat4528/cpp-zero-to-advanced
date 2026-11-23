# Project 06: Car-Rental-System

## 1. Project Overview
`Car-Rental-System` is an object-oriented automotive fleet management and reservation engine backed by an embedded SQLite database. It models distinct vehicle categories with polymorphic rental rate calculation, handles customer records, and provides conflict detection to eliminate double bookings.

## 2. Problem Statement
Car rental operations must handle dynamic pricing based on vehicle classification, track mileage tiers and insurance riders, and strictly prevent overlapping booking reservations across fleet inventory.

## 3. Objectives
- Design a polymorphic vehicle class hierarchy (`EconomyCar`, `SuvCar`, `LuxuryCar`) implementing tailored rental pricing algorithms, mileage allowances, and insurance options.
- Build an SQLite relational store tracking vehicles, verified customer drivers, and scheduled reservations.
- Implement an interval intersection collision detection algorithm preventing date-range conflicts.
- Facilitate end-to-end rental state progression (`ACTIVE` to `RETURNED`).

## 4. Features
- **Polymorphic Pricing Models**:
  - Economy: $45/day base, $12 insurance, 100 free miles/day ($0.15/mile excess).
  - SUV: $75/day base, $18 insurance, 150 free miles/day ($0.25/mile excess).
  - Luxury: $140/day base, $35 concierge insurance, 75 free miles/day ($0.50/mile excess).
- **Date Range Collision Prevention**: Validates `[start_date, end_date]` against existing bookings for the specified VIN.
- **Relational Integrity**: Foreign keys binding reservations to fleet VINs and customer driver's licenses.
- **In-Memory Testing Support**: Zero disk I/O test suite running against `:memory:` SQLite instances.

## 5. Technology Stack
- **Language**: C++17
- **Compiler**: GCC 11+, Clang 14+, MSVC 2022
- **Build System**: CMake 3.20+
- **Database**: SQLite3 via C API (`SQLite3::SQLite3`)
- **Testing**: doctest 2.4+ via CTest
- **External Services**: None (100% offline)

## 6. Folder Structure
```
06-Car-Rental-System/
├── include/
│   ├── RentalSystem.hpp
│   └── Vehicle.hpp
├── src/
│   ├── RentalSystem.cpp
│   ├── Vehicle.cpp
│   └── main.cpp
├── tests/
│   └── test_rental_system.cpp
├── CMakeLists.txt
├── vcpkg.json
├── README.md
└── .gitignore
```

## 7. Installation / Prerequisites
- CMake 3.20+
- C++17 compliant compiler
- SQLite3 development library and headers

## 8. Build and Run Instructions
### Windows & Linux
```bash
cmake -S . -B build -G Ninja
cmake --build build

# Launch application console:
./build/CarRentalSystem

# Run automated headless verification:
./build/CarRentalSystem --test-run
```

## 9. Testing Instructions
```bash
ctest --test-dir build --output-on-failure
```

## 10. Console Output Sample
```
=================================================================
               ENTERPRISE CAR RENTAL FLEET SYSTEM                
=================================================================
[Test-Run Mode] Populating fleet, booking reservations, and testing collision prevention...
Booking successful! Reservation code: RES-VIN-1001
Is VIN101 available June 3-7? NO (Collision detected correctly)
Active reservations: 1
```

## 11. Known Limitations
- Dates are modeled as ISO-8601 strings (`YYYY-MM-DD`); time-of-day precision (hourly pickup/drop-off) is not evaluated.

## 12. Testing Status
- **Built**: Yes (Compiled cleanly with GCC 16.1.0 on Windows, 0 warnings).
- **Run**: Verified interactive execution and `--test-run` headless mode.
- **Tested**: 2 multi-subcase test suites (polymorphic calculations and reservation collision lifecycle) passing under CTest.
