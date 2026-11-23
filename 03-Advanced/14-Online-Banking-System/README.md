# 14-Online-Banking-System

## 1. Project Overview
> **EDUCATIONAL DISCLAIMER**:
> This software is strictly an educational system architecture demonstration. It simulates core banking concepts (ACID transactions, ledger reconciliation, balance locking). No real money, financial institutions, or actual cardholder data are involved.

14-Online-Banking-System is a multi-threaded, client-server financial transaction system implemented in standard C++17 with SQLite3 database backing. It demonstrates strict ACID properties (Atomicity, Consistency, Isolation, Durability) for inter-account fund transfers, atomic debit/credit operations, double-entry ledger journaling, and deadlock-free immediate transaction locks.

## 2. Problem Statement
Distributed financial applications require strict transactional atomicity: if any step in an account transfer fails (network drop, overdraft, crash), the entire transfer must roll back, ensuring no money is created or destroyed. Furthermore, balances must be represented with exact integer precision (cents) rather than floating-point numbers to prevent fractional rounding drift.

## 3. Objectives
- Implement relational banking tables in SQLite3 with foreign key and unique reference constraints.
- Guarantee ACID atomicity via `BEGIN IMMEDIATE TRANSACTION`, atomic debit/credit checks, and automatic `ROLLBACK` on constraint failure.
- Prevent rounding inaccuracies by maintaining all ledger balances in 64-bit integer cents (`int64_t`).
- Build a client-server socket protocol (`SocketCompat.hpp`) supporting PIN authentication, balance queries, and fund transfer execution.
- Provide automated headless CI verification via `--test-run` and doctest unit tests.

## 4. Features
- **ACID Transaction Engine**: Immediate transaction locks ensure serializable isolation during concurrent transfer execution.
- **Fixed-Point Precision**: Financial balances expressed in integer cents (`$1.00 = 100 cents`), preventing IEEE-754 floating-point errors.
- **Audit Ledger Logging**: Every settled transaction generates an immutable journal entry with a unique reference ID.
- **Overdraft Protection**: Rejects transfers exceeding available balance with zero state modification.

## 5. Technology Stack
- **Language**: C++17 (`std::thread`, `std::mutex`, `std::vector`, `std::string`, `std::optional`, `std::cstdint`).
- **Database**: SQLite3 via C API (`sqlite3_prepare_v2`, `sqlite3_step`, `sqlite3_bind_int64`).
- **Networking**: Winsock2 (`ws2_32`) on Windows; POSIX Sockets on Linux.
- **External Services**: None. Fully offline and self-contained.
- **Testing**: `doctest` unit testing framework.

## 6. Folder Structure
```
14-Online-Banking-System/
├── CMakeLists.txt
├── README.md
├── .gitignore
├── include/
│   ├── BankingClient.hpp
│   ├── BankingDatabase.hpp
│   ├── BankingServer.hpp
│   └── SocketCompat.hpp
├── src/
│   ├── BankingClient.cpp
│   ├── BankingDatabase.cpp
│   ├── BankingServer.cpp
│   ├── client_main.cpp
│   └── server_main.cpp
└── tests/
    └── test_banking_system.cpp
```

## 7. Installation / Prerequisites
- CMake 3.20 or newer.
- GCC 11+, Clang 14+, or MSVC 2022.
- SQLite3 development library.
- doctest headers.

## 8. Build and Run Instructions
### Windows & Linux Build
```bash
cmake -S . -B build -G Ninja
cmake --build build
```

### Running the Server
```bash
./build/BankingServer --port 9050
```

### Running the Client Terminal
```bash
./build/BankingClient
```

### Automated Verification Mode
```bash
./build/BankingServer --test-run
./build/BankingClient --test-run
```

## 9. Testing Instructions
Run CTest:
```bash
ctest --test-dir build --output-on-failure
```

## 10. Console Output Sample
```
========================================
       Online Banking Client Terminal   
========================================
[Test Mode] Running automated banking transaction simulation...
Logging in as Alice (ACC-TEST-A)...
Initial Balance: $1000
Executing $250.00 transfer to Bob (ACC-TEST-B)...
Transfer SUCCESS! Transaction Reference: TX-G981LA4Z
Updated Balance: $750
Attempting overdraft ($5,000.00) transfer...
Overdraft attempt: SUCCESS (ACID REJECTED WITH ROLLBACK) -> Insufficient funds
[Test Mode] Banking client-server test passed.
```

## 11. Known Limitations
- Educational demonstration; production core banking requires hardware security modules (HSM) for PIN encryption and two-phase commit (2PC) protocols across distributed ledgers.

## 12. Testing Status
- **Built**: Yes, with GCC 16.1.0 on Windows linking SQLite3 and `ws2_32` with 0 warnings.
- **Run**: Verified multi-threaded transfers, ACID rollback on overdraft, and balance consistency with `--test-run`.
- **Tested**: All unit test suites passed under CTest.
