# 20-Hotel-Booking-System

A high-integrity hotel reservation, room inventory, and stay interval conflict engine in modern C++17 backed by an ACID-compliant SQLite relational database.

## 1. Project Overview
The **Hotel Booking System** simulates a real-world enterprise reservation system that manages room inventory, guest profiles, rates, and stay reservations. The system enforces strict temporal non-overlapping constraints using mathematical interval intersection queries executed within atomic transactions to prevent double-booking anomalies under concurrent operations.

## 2. Problem Statement
In hospitality management platforms, room overbooking is a critical operational failure. Traditional naive interval checking frequently suffers from edge-case vulnerabilities:
- Exact date matches.
- Sub-interval bookings (a stay inside another stay).
- Super-interval bookings (a stay enclosing another stay).
- Partial start or end date overlaps.
- Race conditions during simultaneous booking attempts.

The challenge is to implement a robust, transaction-safe temporal collision detection algorithm that guarantees room availability and ensures seamless adjacent check-in/check-out boundaries ($CheckOut_1 = CheckIn_2$).

## 3. Objectives
- Implement an interval intersection detector: Two date intervals $[A_{in}, A_{out})$ and $[B_{in}, B_{out})$ overlap if and only if $\neg(A_{out} \le B_{in} \lor A_{in} \ge B_{out})$.
- Provide ACID-compliant reservations via SQLite transactions (`BEGIN IMMEDIATE TRANSACTION`).
- Maintain an accurate room inventory with categorized pricing (in integer cents) and occupancy limits.
- Support real-time availability querying by date range and optional room classification (Single, Double, Suite, Penthouse).
- Implement reservation cancellation with immediate availability restoration.

## 4. Features
- **Temporal Interval Conflict Prevention**: Guaranteed prevention of double-bookings across all 5 overlap permutations.
- **Adjacent Stay Support**: Allows back-to-back reservations where a guest checks out on the morning another guest checks in.
- **ACID Transaction Isolation**: Uses immediate SQLite write locks to eliminate write-after-read race conditions.
- **Dynamic Availability Filtering**: Instant discovery of available rooms for a given date window, filterable by luxury category.
- **Financial Precision**: Room tariffs and booking totals calculated in integer cents to eliminate floating-point rounding errors.
- **Reservation Tracking**: Unique alphanumeric reference codes (`HB-XXXXXXX`) for guest lookup and check-in workflows.

## 5. Technology Stack
- **Language**: C++17 (`std::optional`, `<chrono>`, structured records).
- **Database**: SQLite3 (native C API wrapper `sqlite3.h`).
  - *Why needed*: Required for relational data modeling, indexing, and ACID transaction semantics.
- **Testing Framework**: doctest (unit tests and boundary case verification).
- **Build System**: CMake 3.20+.
- **External Services / Internet**: None required. Runs fully local with in-memory or file-backed database storage.

## 6. Folder Structure
```
20-Hotel-Booking-System/
├── CMakeLists.txt
├── .gitignore
├── README.md
├── include/
│   ├── HotelDatabase.hpp
│   └── HotelManager.hpp
├── src/
│   ├── HotelDatabase.cpp
│   ├── HotelManager.cpp
│   └── main.cpp
└── tests/
    └── test_hotel_booking.cpp
```

## 7. Installation / Prerequisites
- **Compiler**: GCC 11+, Clang 14+, or MSVC 2022 supporting C++17.
- **CMake**: Version 3.20 or newer.
- **SQLite3**: Development headers and library (`libsqlite3-dev` on Debian/Ubuntu, `mingw-w64-x86_64-sqlite3` on MSYS2, or vcpkg).
- **doctest**: Installed in system include path or via MSYS2 / package manager.

## 8. Build and Run Instructions

### Linux / macOS
```bash
cmake -S . -B build
cmake --build build
./build/HotelReservation
```

### Windows (MSYS2 MinGW / Ninja)
```powershell
cmake -S . -B build -G Ninja
cmake --build build
.\build\HotelReservation.exe
```

## 9. Testing Instructions
Run unit tests with CTest:
```bash
ctest --test-dir build --output-on-failure
```

Direct test binary execution:
```bash
./build/test_hotel_booking
```

## 10. Console Output Demo
```text
========================================
   Hotel Reservation & Inventory Engine 
  (SQLite Date Interval Conflict Check) 
========================================
Hotel initialized with 4 inventory rooms.
Booking 1 (John in Suite 301, 2026-12-10 -> 2026-12-15): CONFIRMED (Ref: HB-K82W19P)
Booking 2 (Jane in Suite 301, 2026-12-12 -> 2026-12-18): REJECTED (SUCCESS: Room is already booked for the selected date interval)
Booking 3 (Adjacent: 2026-12-15 -> 2026-12-20): CONFIRMED (Ref: HB-9TX0LA4)

Rooms Available between 2026-12-11 and 2026-12-14: 3
  - Room #101 (Single) Price: $99.00/night
  - Room #201 (Double) Price: $149.00/night
  - Room #401 (Penthouse) Price: $599.00/night
```

## 11. Known Limitations
- Dates are modeled as ISO-8601 strings (`YYYY-MM-DD`). Check-in and check-out times are fixed to standard 14:00 check-in and 11:00 check-out semantics.
- Currency is stored in integer cents (USD standard); multi-currency conversion is not implemented.
- Housekeeping maintenance blocks are represented as system-level reservations rather than a separate state machine.

## 12. Testing Status
- **Built**: Yes, compiled with GCC 16.1.0 and CMake with `-Wall -Wextra -Wpedantic` (0 warnings).
- **Run**: Verified via `./build/HotelReservation --test-run`.
- **Tested**: 3 comprehensive doctest suites passing 100% of boundary condition assertions (exact match overlap, sub-interval, super-interval, partial boundary, adjacent non-overlapping, invalid chronological order, and cancellation re-availability).
