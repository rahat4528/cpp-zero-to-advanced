# 06-Library-Manager-CLI

A file-backed Library Catalog and Circulation Management CLI application built with modern C++17.

---

## 1. Project Overview
Library Manager CLI manages book catalogs, borrower circulation states, availability queries, and multi-attribute book searches with persistent text file storage.

## 2. Problem Statement
Managing a physical or digital collection of books requires tracking circulation status (available vs checked out), borrower identities, catalog numbers, and categorical search indexes with deterministic persistence.

## 3. Objectives
- Encapsulate catalog management inside an OOP `LibraryManager` class.
- Enforce strict circulation states (prevent double checkout or invalid returns).
- Implement multi-attribute case-insensitive searching.
- Provide structured pipe-delimited file serialization.
- Provide a robust CLI interface.

## 4. Features
- **Circulation Lifecycle**: State checking prevents checking out already borrowed books or returning unborrowed books.
- **Search Engine**: Case-insensitive substring searches across Title, Author, ISBN, Category, and Borrower.
- **File Persistence**: Automatic saving and loading with header validation.
- **Scriptable CLI**: Automated flags for integration testing (`--add`, `--borrow`, `--return`, `--list`, `--search`).

## 5. Technology Stack
- **Language**: C++17 (`std::vector`, `std::fstream`, `std::optional`, `std::string`)
- **Build System**: CMake 3.20+
- **Testing Framework**: doctest with CTest
- **Database Needed**: No. Plaintext file storage.
- **Internet / External Service Needed**: No.

## 6. Folder Structure
```
06-Library-Manager-CLI/
├── include/
│   └── LibraryManager.h       # Book struct and LibraryManager class API
├── src/
│   ├── LibraryManager.cpp     # Catalog operations and file I/O
│   └── main.cpp              # CLI driver and table display
├── tests/
│   └── test_library_manager.cpp # Unit test suite
├── CMakeLists.txt            # CMake build configuration
├── .gitignore                # Build ignore rules
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

# Add books
.\build\LibraryManager.exe --file catalog.txt --add "The C++ Programming Language|Bjarne Stroustrup|9780321563842|Programming"
.\build\LibraryManager.exe --file catalog.txt --add "Design Patterns|Gang of Four|9780201633610|Architecture"

# Borrow a book
.\build\LibraryManager.exe --file catalog.txt --borrow B101 --patron "Ada Lovelace"

# List catalog
.\build\LibraryManager.exe --file catalog.txt --list

# Return a book
.\build\LibraryManager.exe --file catalog.txt --return B101
```

### Linux / macOS
```bash
cmake -S . -B build -G Ninja
cmake --build build

./build/LibraryManager --file catalog.txt --list
```

## 9. Testing Instructions
Run tests with CTest:
```bash
ctest --test-dir build --output-on-failure
```

## 10. Demo / Console Output Sample
```
====================================================================================================
ID      TITLE                          AUTHOR                 ISBN            STATUS      BORROWER
----------------------------------------------------------------------------------------------------
B101    The C++ Programming Language   Bjarne Stroustrup      9780321563842   CHECKED OUT Ada Lovelace
B102    Design Patterns                Gang of Four           9780201633610   AVAILABLE   -
====================================================================================================
```

## 11. Known Limitations
- Does not currently compute overdue fee penalties (calculated in Intermediate projects).
- Single-catalog file without concurrency locking.

## 12. Testing Status
- **Built**: Yes, compiled with GCC 16.1.0 using `-Wall -Wextra -Wpedantic`.
- **Run**: Yes, verified add, borrow, return, search, and list.
- **Tested**: Yes, 4 test suites passing via CTest.
- **Hardware/Credentials Limitations**: None.
