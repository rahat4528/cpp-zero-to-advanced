# 04-Contact-Saver-CLI

A file-backed Contact Manager and Address Book Command-Line Application built with modern C++17.

---

## 1. Project Overview
Contact Saver CLI provides contact book management with CSV serialization, field validation (phone numbers and emails), multi-field substring searching, and automatic alphabetical sorting.

## 2. Problem Statement
Maintaining contact information without structured persistence leads to data corruption or loss. Users require a lightweight, zero-dependency tool that can validate telephone and email formats, persist records to standard CSV files, and query contacts instantly.

## 3. Objectives
- Model contact data using modern C++ structs and classes.
- Implement robust CSV persistence (file I/O streams with header management).
- Validate telephone numbers and RFC-compliant email strings.
- Provide fast case-insensitive multi-field search across names, phone numbers, emails, and notes.

## 4. Features
- **Field Validation**: Rejects invalid phone numbers (<7 digits) and malformed email addresses.
- **CSV Storage**: Reads and writes standard CSV files (`contacts.csv`).
- **Alphabetical Sorting**: Automatically keeps contacts sorted by name.
- **Search Engine**: Substring searching across all fields simultaneously.
- **CLI Automation**: Scriptable via `--add`, `--list`, `--search`, `--delete`, and `--file`.

## 5. Technology Stack
- **Language**: C++17 (`std::vector`, `std::fstream`, `std::stringstream`, `std::optional`)
- **Build System**: CMake 3.20+
- **Testing Framework**: doctest with CTest
- **Database Needed**: No. Plaintext CSV file persistence.
- **Internet / External Service Needed**: No.

## 6. Folder Structure
```
04-Contact-Saver-CLI/
├── include/
│   └── ContactSaver.h         # Contact struct & ContactBook class API
├── src/
│   ├── ContactSaver.cpp       # CRUD operations, validation, CSV parsing
│   └── main.cpp              # CLI driver and table formatter
├── tests/
│   └── test_contact_saver.cpp # doctest unit tests
├── CMakeLists.txt            # CMake build file
├── .gitignore                # Git ignore rules
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

# Add a contact
.\build\ContactSaver.exe --file my_contacts.csv --add "Alan Turing|555-019-2831|alan@bletchley.uk|Mathematician"
.\build\ContactSaver.exe --file my_contacts.csv --add "Ada Lovelace|555-023-9912|ada@analytics.org|First Programmer"

# List contacts
.\build\ContactSaver.exe --file my_contacts.csv --list

# Search contacts
.\build\ContactSaver.exe --file my_contacts.csv --search "turing"
```

### Linux / macOS
```bash
cmake -S . -B build -G Ninja
cmake --build build

./build/ContactSaver --file my_contacts.csv --list
```

## 9. Testing Instructions
Execute tests via CTest:
```bash
ctest --test-dir build --output-on-failure
```

## 10. Demo / Console Output Sample
```
========================================================================================
ID      NAME                    PHONE             EMAIL                     NOTES
----------------------------------------------------------------------------------------
c_2     Ada Lovelace            555-023-9912      ada@analytics.org         First Programmer
c_1     Alan Turing             555-019-2831      alan@bletchley.uk         Mathematician
========================================================================================
```

## 11. Known Limitations
- CSV escaping does not currently support embedded commas inside note strings (delimiters are commas).
- Single-user local file access without file locking.

## 12. Testing Status
- **Built**: Yes, compiled cleanly with GCC 16.1.0 (`-Wall -Wextra -Wpedantic`).
- **Run**: Yes, tested `--add`, `--list`, `--search`, and CSV roundtrips.
- **Tested**: Yes, 4 test suites passing via CTest.
- **Hardware/Credentials Limitations**: None.
