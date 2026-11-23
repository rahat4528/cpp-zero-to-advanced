# 17-ATM-Machine-Simulation

An educational Finite State Machine (FSM) simulating automated teller machine transactions, PIN authentication, cash dispensing, and audit trails built with modern C++17.

---

## 1. Project Overview
ATM Machine Simulation models the operational lifecycle of a banking terminal. It implements a strict state transition machine, enforces security policies (3-attempt PIN lockout, daily withdrawal limits), breaks down cash withdrawals into physical banknote denominations ($100, $50, $20), and tracks transaction histories.

## 2. Problem Statement
Financial terminals require rigorous state modeling to prevent unauthorized access or state race conditions (e.g. attempting withdrawals without an authenticated card session). System designs must also handle hardware constraints such as fixed-denomination cash dispenser algorithms.

## 3. Objectives
- Implement an explicit Finite State Machine (`Idle` -> `CardInserted` -> `Authenticated` -> `SessionEnded`).
- Enforce PIN security policies with automatic account lockout after 3 consecutive failed attempts.
- Calculate optimal banknote distributions ($100, $50, $20) for cash dispensing.
- Maintain an in-memory chronological transaction log for mini-statements.

## 4. Features
- **Strict FSM Architecture**: Rejects out-of-order operations.
- **Cash Dispenser Algorithms**: Decomposes withdrawals into optimal physical note counts.
- **Account Protection**: Daily limits and lockout triggers.
- **Scriptable CLI**: Automated test execution via `--account`, `--pin`, and `--actions`.

## 5. Technology Stack
- **Language**: C++17 (`std::vector`, `std::optional`, `std::string`, `std::ostringstream`)
- **Build System**: CMake 3.20+
- **Testing Framework**: doctest with CTest
- **Database Needed**: No. In-memory educational simulation (no real money or banking data).
- **Internet / External Service Needed**: No.

## 6. Folder Structure
```
17-ATM-Machine-Simulation/
├── include/
│   └── AtmSimulation.h          # FSM enums, Account model, and AtmEngine API
├── src/
│   ├── AtmSimulation.cpp        # State transitions and banknote dispensing logic
│   └── main.cpp                 # CLI driver
├── tests/
│   └── test_atm_simulation.cpp  # doctest unit test suite
├── CMakeLists.txt               # CMake build configuration
├── .gitignore                   # Build ignore rules
└── README.md                    # Project documentation
```

## 7. Installation / Prerequisites
- GCC 11+, Clang 14+, or MSVC 2022
- CMake 3.20+ and Ninja (or Make)

## 8. Build and Run Instructions

### Windows (PowerShell)
```powershell
cmake -S . -B build -G Ninja
cmake --build build

# Automated transaction batch execution
.\build\AtmSimulation.exe --account 12345678 --pin 4321 --actions "balance;withdraw:170;deposit:50;statement"
```

### Linux / macOS
```bash
cmake -S . -B build -G Ninja
cmake --build build

./build/AtmSimulation --account 12345678 --pin 4321 --actions "balance;withdraw:100;statement"
```

## 9. Testing Instructions
Run tests with CTest:
```bash
ctest --test-dir build --output-on-failure
```

## 10. Demo / Console Output Sample
```
$ ./build/AtmSimulation --account 12345678 --pin 4321 --actions "balance;withdraw:170;deposit:50;statement"
=== ATM Terminal Simulation ===
Card authenticated successfully. Processing transaction batch...
>> Current Balance: $2500.00
>> Dispensed $170.00 (1x$100, 1x$50, 1x$20)
>> Deposited $50.00

>> MINI-STATEMENT:
   [03:30:15] ATM Cash Withdrawal Amount: $170.00 | Balance: $2330.00
   [03:30:15] ATM Cash/Check Deposit Amount: $50.00 | Balance: $2380.00
Session completed. Card ejected safely.
```

## 11. Known Limitations
- Educational simulation with in-memory ledger (for persistent multi-client ACID banking, see `03-Advanced/14-Online-Banking-System`).
- Dispenses denominations down to $20 bills; single dollar bills or coins not modeled.

## 12. Testing Status
- **Built**: Yes, compiled cleanly with GCC 16.1.0 using `-Wall -Wextra -Wpedantic`.
- **Run**: Yes, verified card insertion, PIN authentication, 3-attempt lockout, and cash dispensing.
- **Tested**: Yes, 4 test suites passing via CTest.
- **Hardware/Credentials Limitations**: None.
