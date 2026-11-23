# 02-Student-Record-Database

A production-grade relational Student Information System backed by the SQLite3 C API and parameterized prepared statements built with modern C++17.

---

## 1. Project Overview
Student Record Database manages student enrollment records, GPA analytics, and major distributions. It utilizes SQLite3 directly through its native C API, implements strict parameterized query binding to eliminate SQL injection vulnerabilities, and uses ACID transactions (`BEGIN`, `COMMIT`, `ROLLBACK`).

## 2. Problem Statement
Plain text or CSV storage lacks ACID transactional guarantees, index-accelerated searching, and concurrency protection. Building database drivers in C++ requires mastering SQLite handles, statement compilation (`sqlite3_prepare_v2`), parameter binding, column extraction, and resource finalization without memory leaks.

## 3. Objectives
- Encapsulate the SQLite3 C API inside an RAII `StudentDatabase` class.
- Eliminate SQL injection attacks via parameterized prepared statements (`sqlite3_bind_*`).
- Implement indexing on frequent lookup fields (e.g. `major`, `name`).
- Support atomic multi-statement transactions with rollback safety.
- Expose statistical metrics (average GPA, highest/lowest GPA, credit totals).

## 4. Features
- **Parameterized Queries**: 100% defense against SQL injection.
- **Relational Indices**: Indexed query execution on primary and search fields.
- **ACID Transactions**: Transactional integrity with rollback capabilities.
- **In-Memory & File Backends**: Supports `:memory:` for testing or persistent `.db` files.
- **CLI Automation**: Complete management via `--add`, `--list`, `--search`, `--update-gpa`, `--delete`, and `--stats`.

## 5. Technology Stack
- **Language**: C++17 (`std::vector`, `std::optional`, `std::string`, `std::unique_ptr`)
- **Database**: SQLite 3 C API (`sqlite3_open`, `sqlite3_prepare_v2`, `sqlite3_step`). *Rationale*: Embedded, serverless, self-contained relational SQL database with zero external network daemon requirements.
- **Dependency Management**: `vcpkg.json` provided; discovered via CMake `find_package(SQLite3 REQUIRED)`.
- **Build System**: CMake 3.20+
- **Testing Framework**: doctest with CTest
- **Internet / External Service Needed**: No. Operates locally on disk or in-memory.

## 6. Folder Structure
```
02-Student-Record-Database/
├── include/
│   └── StudentDatabase.h       # Student model, AcademicStats, and DB interface
├── src/
│   ├── StudentDatabase.cpp     # SQLite3 prepared statements & transaction logic
│   └── main.cpp                # CLI driver and report formatter
├── tests/
│   └── test_student_db.cpp     # doctest unit test suite (runs on :memory: DB)
├── vcpkg.json                  # Manifest declaring sqlite3 dependency
├── CMakeLists.txt              # CMake build configuration
├── .gitignore                  # Build ignore rules
└── README.md                   # Project documentation
```

## 7. Installation / Prerequisites
- GCC 11+, Clang 14+, or MSVC 2022
- SQLite3 development headers and library (`libsqlite3`)
- CMake 3.20+ and Ninja (or Make)

## 8. Build and Run Instructions

### Windows (PowerShell)
```powershell
cmake -S . -B build -G Ninja
cmake --build build

# Enroll a student
.\build\StudentRecordDatabase.exe --db university.db --add "STU101|Grace|Hopper|grace@navy.mil|Computer Science|3.95|120"
.\build\StudentRecordDatabase.exe --db university.db --add "STU102|Alan|Turing|alan@bletchley.uk|Mathematics|4.00|130"

# List all students
.\build\StudentRecordDatabase.exe --db university.db --list

# Compute academic metrics
.\build\StudentRecordDatabase.exe --db university.db --stats
```

### Linux / macOS
```bash
cmake -S . -B build -G Ninja
cmake --build build

./build/StudentRecordDatabase --db university.db --list
```

## 9. Testing Instructions
Run tests with CTest:
```bash
ctest --test-dir build --output-on-failure
```

## 10. Demo / Console Output Sample
```
$ ./build/StudentRecordDatabase --db university.db --stats

========================================
       ACADEMIC DATABASE METRICS        
========================================
Total Enrolled Students : 2
Average Cumulative GPA  : 3.98
Highest Student GPA     : 4.00
Lowest Student GPA      : 3.95
Total Completed Credits : 250
========================================
```

## 11. Known Limitations
- Single-process database file access (SQLite standard lock behavior).
- Student photo or binary BLOB storage is not implemented.

## 12. Testing Status
- **Built**: Yes, compiled cleanly with GCC 16.1.0 using `-Wall -Wextra -Wpedantic`.
- **Run**: Yes, verified SQL queries, student enrollment, GPA updates, and statistics.
- **Tested**: Yes, 5 unit test suites passing via CTest on `:memory:` database.
- **Hardware/Credentials Limitations**: None.
