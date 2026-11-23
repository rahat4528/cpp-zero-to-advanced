# Project 20: File-Explorer-GUI

## 1. Project Overview
`File-Explorer-GUI` is a file system exploration and metadata inspection utility powered by C++17 `std::filesystem`. It provides directory traversal, multi-column sorting (name, file size), recursive folder capacity aggregation, and extension filtration.

## 2. Problem Statement
Managing local disk file hierarchies requires robust traversal engines that gracefully handle permission-denied barriers, symbolic link loops, file size unit formatting, and sorting algorithms that prioritize directory nodes over leaf files.

## 3. Objectives
- Implement directory navigation and recursive tree scanning using C++17 `std::filesystem`.
- Format raw 64-bit byte counts into human-readable notation (`B`, `KB`, `MB`, `GB`, `TB`).
- Offer sorting mechanisms (alphabetical, size ascending/descending) with directories placed ahead of files.
- Deliver automated unit tests operating against dynamically created temporary sandbox folders.

## 4. Features
- **Hierarchical Navigation**: Canonical path tracking with parent navigation (`navigateUp`) and folder transitions (`navigateTo`).
- **Flexible Sorting**: Name ascending/descending, size ascending/descending, preserving directory group priority.
- **Recursive Space Aggregator**: Accurately sums disk storage footprint across nested directory hierarchies.
- **Extension Filter**: Real-time filtering by file extensions (e.g. `.cpp`, `.txt`).

## 5. Technology Stack
- **Language**: C++17
- **Compiler**: GCC 11+, Clang 14+, MSVC 2022
- **Build System**: CMake 3.20+
- **Standard Library**: `<filesystem>`
- **Testing**: doctest 2.4+ via CTest
- **GUI Toolkit Rationale**: Implemented with an interactive decoupled TUI dashboard; separating the explorer filesystem core allows direct plug-and-play binding to Qt Widgets (`QTreeView`) or WinUI.

## 6. Folder Structure
```
20-File-Explorer-GUI/
├── include/
│   └── FileExplorer.hpp
├── src/
│   ├── FileExplorer.cpp
│   └── main.cpp
├── tests/
│   └── test_file_explorer.cpp
├── CMakeLists.txt
├── README.md
└── .gitignore
```

## 7. Installation / Prerequisites
- CMake 3.20+
- C++17 compliant compiler
- Ninja or Make

## 8. Build and Run Instructions
### Windows & Linux
```bash
cmake -S . -B build -G Ninja
cmake --build build

# Headless scenario verification:
./build/FileExplorerApp --test-run
```

## 9. Testing Instructions
```bash
ctest --test-dir build --output-on-failure
```

## 10. Console Output Sample
```
=================================================================
           FILE SYSTEM EXPLORER & METRICS INSPECTOR              
=================================================================
[Test-Run Mode] Navigating local directory structure...
Current Directory: C:\Users\Rahat\cpp-zero-to-advanced\02-Intermediate\20-File-Explorer-GUI

Directory Contents (7 items):
-----------------------------------------------------------------
NAME                               TYPE        SIZE           
-----------------------------------------------------------------
include                            [DIR]       <DIR>          
src                                [DIR]       <DIR>          
tests                              [DIR]       <DIR>          
CMakeLists.txt                     [FILE]      1.1 KB         
README.md                          [FILE]      2.8 KB         
.gitignore                         [FILE]      45.0 B         
-----------------------------------------------------------------
[Test-Run Mode] Completed successfully.
```

## 11. Known Limitations
- Graphical thumbnail generation for image/video files requires platform image rasterizers.

## 12. Testing Status
- **Built**: Yes (Clean compilation under GCC 16.1.0 on Windows, 0 warnings).
- **Run**: Verified `--test-run` and directory inspection.
- **Tested**: 2 multi-subcase test suites passing under CTest.
