# 09-Crime-Record-Manager

## 1. Project Overview
> **DISCLAIMER**: This application is strictly an educational software project utilizing **100% fictional sample records and synthetic test identifiers**. No real law enforcement data, real individuals, or real incidents are contained within or managed by this software.

09-Crime-Record-Manager is a secure incident and case record management application developed in modern C++17. It couples relational SQLite data storage with multi-tier Role-Based Access Control (RBAC: Officer, Detective, Admin), cryptographic password hashing (SHA-256 + 16-byte random salt), security clearance classification filtering, and a tamper-evident Merkle hash-chained audit log.

## 2. Problem Statement
Law enforcement record management systems handle sensitive public safety data that require strict access boundaries, chain of custody tracking, and non-repudiation. Uncontrolled data modifications or undetected database record alteration compromise legal investigations. This application enforces RBAC authorization barriers and cryptographic audit integrity checking.

## 3. Objectives
- Implement Role-Based Access Control (`Officer`, `Detective`, `Admin`) governing case creation, modification, and status updates.
- Enforce security clearance filtering (`Public`, `Confidential`, `TopSecret`), ensuring patrol officers only access public incidents while detectives access confidential investigative materials.
- Implement an immutable cryptographic hash chain where every event records `SHA-256(prev_hash + metadata)` to detect any audit tampering.
- Provide automated CI smoke verification (`--test-run`) and full doctest coverage.

## 4. Features
- **Multi-Role Authentication**: Passwords salted and hashed with SHA-256; active sessions protected by randomized session tokens.
- **Clearance Level Visibility**: In-database query filtering prevents unauthorized clearance leakage.
- **Cryptographic Audit Trail**: Blockchain-style sequential hashing linking each log entry to the genesis block and previous entry hash.
- **ACID Transaction Guarantees**: Relational consistency backed by SQLite3.
- **Fictional Data Sandbox**: Clean synthetic schemas for testing without external baggage.

## 5. Technology Stack
- **Language**: C++17 (`std::vector`, `std::string`, `std::optional`, `std::chrono`, `std::random_device`).
- **Database**: SQLite3 via C API (`sqlite3_prepare_v2`, `sqlite3_step`).
- **Cryptography**: OpenSSL `libcrypto` (`EVP_MD_CTX`, `EVP_sha256`).
- **External Services**: None. Fully offline and self-contained.
- **Testing**: `doctest` unit testing framework.

## 6. Folder Structure
```
09-Crime-Record-Manager/
├── CMakeLists.txt
├── README.md
├── .gitignore
├── include/
│   ├── AuthManager.hpp
│   └── CrimeDatabase.hpp
├── src/
│   ├── AuthManager.cpp
│   ├── CrimeDatabase.cpp
│   └── main.cpp
└── tests/
    └── test_crime_manager.cpp
```

## 7. Installation / Prerequisites
- CMake 3.20 or newer.
- GCC 11+, Clang 14+, or MSVC 2022.
- SQLite3 development library.
- OpenSSL development library (`libcrypto`).
- doctest headers.

## 8. Build and Run Instructions
### Windows & Linux Build
```bash
cmake -S . -B build -G Ninja
cmake --build build
```

### Running the Application
```bash
./build/CrimeRecordManager
```

### Automated Verification Mode
```bash
./build/CrimeRecordManager --test-run
```

## 9. Testing Instructions
Run CTest:
```bash
ctest --test-dir build --output-on-failure
```

## 10. Console Output Sample
```
========================================
 Fictional Crime Record Manager (RBAC)  
========================================
Authenticating users...
Officer Unauthorized Update: REJECTED (RBAC SECURE)
Cases visible to Officer: 1 (Public only)
Cases visible to Admin:   2 (Full access)
Cryptographic Merkle Audit Hash Chain: VALID & UNTAMPERED
[Test Mode] Crime record manager RBAC and audit trail verified.
```

## 11. Known Limitations
- Session tokens expire upon process termination in the educational prototype; production deployments typically integrate persistent JWT or OAuth2 bearer tokens.

## 12. Testing Status
- **Built**: Yes, with GCC 16.1.0 on Windows linking SQLite3 and OpenSSL with 0 warnings.
- **Run**: Verified multi-role authentication, RBAC authorization blocks, and audit hash-chain verification with `--test-run`.
- **Tested**: All unit test suites passed under CTest.
