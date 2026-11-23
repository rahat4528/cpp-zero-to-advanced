# Project 17: Phonebook-with-Database

## 1. Project Overview
`Phonebook-with-Database` is a relational contact management system powered by an embedded SQLite database. It provides multi-attribute contact indexing, parameterized substring query lookups, favorite priority filtering, and vCard 3.0 standard serialization.

## 2. Problem Statement
Personal and organizational contact management requires durable ACID storage, parameterized querying to prevent SQL injection vulnerabilities, fast indexed lookups across large telephone datasets, and standard vCard interchange formats.

## 3. Objectives
- Establish an SQLite schema indexing names and telephone records.
- Implement parameterized prepared statements for inserting, updating, querying, and deleting contact records.
- Build vCard 3.0 (`.vcf`) serialization and parser deserialization routines for cross-platform address book synchronization.
- Enable fast in-memory (`:memory:`) test execution for automated CI workflows.

## 4. Features
- **ACID Persistence**: Safe concurrent transactions with SQLite index optimization (`idx_contacts_name`, `idx_contacts_phone`).
- **Parameterized Search**: Substring searching across first names, last names, and phone numbers.
- **vCard 3.0 Import/Export**: Complies with RFC 2426 format (`FN`, `N`, `ORG`, `TEL`, `EMAIL`, `NOTE`).
- **Favorites Filter**: Dedicated index query isolating starred VIP contacts.

## 5. Technology Stack
- **Language**: C++17
- **Compiler**: GCC 11+, Clang 14+, MSVC 2022
- **Build System**: CMake 3.20+
- **Database**: SQLite3 via C API (`SQLite3::SQLite3`)
- **Testing**: doctest 2.4+ via CTest
- **External Services**: None (100% offline)

## 6. Folder Structure
```
17-Phonebook-with-Database/
├── include/
│   └── PhonebookDatabase.hpp
├── src/
│   ├── PhonebookDatabase.cpp
│   └── main.cpp
├── tests/
│   └── test_phonebook.cpp
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

# Launch interactive phonebook:
./build/PhonebookApp

# Run headless scenario verification:
./build/PhonebookApp --test-run
```

## 9. Testing Instructions
```bash
ctest --test-dir build --output-on-failure
```

## 10. Console Output Sample
```
=================================================================
              PHONEBOOK DATABASE WITH vCARD EXPORT               
=================================================================
[Test-Run Mode] Registering contacts, searching, and exporting vCard...
Total Contacts Registered: 3
 [1] [*FAV*] Lovelace, Ada | Phone: +44-20-7946-0912 | Org: Babbage Labs
 [2] [*FAV*] Turing, Alan | Phone: +44-1628-890123 | Org: Gov Code & Cypher
 [3] Shannon, Claude | Phone: +1-617-253-1000 | Org: Bell Labs

Search 'Turing':
  Found: Alan Turing (+44-1628-890123)

Export vCard for Shannon:
BEGIN:VCARD
VERSION:3.0
N:Shannon;Claude;;;
FN:Claude Shannon
ORG:Bell Labs
TEL;TYPE=CELL:+1-617-253-1000
EMAIL;TYPE=INTERNET:shannon@mit.edu
NOTE:Father of Information Theory
END:VCARD
```

## 11. Known Limitations
- Current schema uses single phone number per contact; multiple numbers (Home, Mobile, Work) would require a relational 1-to-many phone numbers table.

## 12. Testing Status
- **Built**: Yes (Clean compilation under GCC 16.1.0 on Windows, 0 warnings).
- **Run**: Verified `--test-run` and interactive launch.
- **Tested**: 4 comprehensive doctest subcases passing under CTest.
