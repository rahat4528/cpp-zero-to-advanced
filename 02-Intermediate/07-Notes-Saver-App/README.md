# Project 07: Notes-Saver-App

## 1. Project Overview
`Notes-Saver-App` is a persistent markdown note management application powered by an embedded SQLite relational database. It provides categorized note organization, tag queries, full-text substring searching, priority pinning, and Markdown export capabilities.

## 2. Problem Statement
Developers and students require a distraction-free, reliable knowledge-base tool that saves structured notes locally with search and export capabilities without relying on third-party cloud servers.

## 3. Objectives
- Provide an SQLite storage engine for markdown documents with structured schemas for titles, markdown bodies, comma-separated tags, and pin states.
- Support keyword full-text searching and tag filtration.
- Offer single-click Markdown export (`.md`) with generated frontmatter metadata.
- Ensure automated test suite runs against in-memory `:memory:` SQLite databases.

## 4. Features
- **Markdown Document Storage**: Formatted text persistence with automatic timestamp tracking (`created_at`, `updated_at`).
- **Priority Pinning**: Pinned notes appear at the top of listings regardless of modification date.
- **Search & Tag Queries**: Query notes by tags (`cpp`, `sql`) or by content substrings.
- **File Export**: Direct serialization to valid standalone Markdown documents.

## 5. Technology Stack
- **Language**: C++17
- **Compiler**: GCC 11+, Clang 14+, MSVC 2022
- **Build System**: CMake 3.20+
- **Database**: SQLite3 via C API (`SQLite3::SQLite3`)
- **Testing**: doctest 2.4+ via CTest
- **GUI Toolkit Rationale**: Implemented with a terminal UI / management console with decoupled persistence for future Qt GUI binding.

## 6. Folder Structure
```
07-Notes-Saver-App/
├── include/
│   └── NotesDatabase.hpp
├── src/
│   ├── NotesDatabase.cpp
│   └── main.cpp
├── tests/
│   └── test_notes.cpp
├── CMakeLists.txt
├── vcpkg.json
├── README.md
└── .gitignore
```

## 7. Installation / Prerequisites
- CMake 3.20+
- C++17 compliant compiler
- SQLite3 development library

## 8. Build and Run Instructions
### Windows & Linux
```bash
cmake -S . -B build -G Ninja
cmake --build build

# Launch application console:
./build/NotesSaver

# Headless verification mode:
./build/NotesSaver --test-run
```

## 9. Testing Instructions
```bash
ctest --test-dir build --output-on-failure
```

## 10. Console Output Sample
```
=================================================================
                   MARKDOWN NOTES SAVER (SQLite)                 
=================================================================
Total Notes: 3
 [1] [PINNED] Modern C++ Concurrency (Tags: cpp,threads,modern)
 [3] CMake Best Practices (Tags: cmake,build,cpp)
 [2] Grocery Shopping List (Tags: personal,shopping)
Search 'concurrency' found: 1 match(es)
```

## 11. Known Limitations
- Rich text WYSIWYG editing requires GUI framework integration (e.g., Qt Widgets `QTextEdit`).

## 12. Testing Status
- **Built**: Yes (Compiled cleanly with GCC 16.1.0 on Windows, 0 warnings).
- **Run**: Verified interactive and `--test-run` modes.
- **Tested**: 5 comprehensive doctest subcases passing under CTest.
