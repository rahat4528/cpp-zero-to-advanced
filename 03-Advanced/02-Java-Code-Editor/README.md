# Project 02: C Plus Plus Code Editor

A 100% native C++ text editor engine featuring a multi-line text buffer, infinite undo/redo revision history, and lexical syntax highlighting for C Plus Plus source code.

> **Language Notice**: This application is built 100% in C Plus Plus (C++17 standard) and highlights C Plus Plus code.

---

## 1. Project Overview
`Java-Code-Editor` is a high-performance C Plus Plus editor backend designed for source code editing. It features a robust text manipulation buffer model, cursor indexing, unbounded undo/redo stacks using the Command Pattern, and a lexical tokenizer that classifies keywords, types, literals, preprocessor directives, and comments with ANSI color highlighting.

## 2. Problem Statement
Implementing a robust code editor requires handling low-level text manipulations without degrading performance during rapid typing, maintaining undo/redo revision invariants without memory leaks, and performing lexical analysis on source code without third-party heavy IDE dependencies.

## 3. Objectives
- Implement an efficient line-indexed `TextBuffer` with boundary checks and newline preservation.
- Provide infinite undo/redo history using command snapshots.
- Build a lexical tokenizer and syntax highlighter (`CppSyntaxHighlighter`) supporting C Plus Plus language tokens.
- Support ANSI colorized terminal rendering.
- Maintain zero compiler warnings with `-Wall -Wextra -Wpedantic`.

## 4. Features
- **Text Buffer Management**: Line insertion, text appending, deletion, and line-level navigation.
- **Command History**: Dual-stack undo/redo with action grouping.
- **Syntax Highlighter**: Identifies C Plus Plus keywords, primitive types, string/char literals, numbers, preprocessor directives, and comments.
- **ANSI Terminal Renderer**: Colorizes code directly in terminal environments.

## 5. Technology Stack
- **Language**: C++17.
- **Architecture**: Object-Oriented Command Pattern & Lexical Analyzer.
- **Testing**: doctest.
- **Build System**: CMake 3.20+.

## 6. Folder Structure
```
02-Java-Code-Editor/
├── CMakeLists.txt
├── README.md
├── .gitignore
├── include/
│   ├── CppSyntaxHighlighter.hpp
│   └── TextBuffer.hpp
├── src/
│   ├── CppSyntaxHighlighter.cpp
│   ├── TextBuffer.cpp
│   └── main.cpp
├── tests/
│   └── test_code_editor.cpp
└── docs/
    └── ARCHITECTURE.md
```

## 7. Prerequisites
- GCC 11+, Clang 14+, or MSVC 2022.
- CMake 3.20+.
- Ninja or Make.

## 8. Build and Run

### Build
```bash
cmake -S . -B build -G Ninja
cmake --build build
```

### Run
```bash
./build/CppCodeEditor --test-run
```

## 9. Testing
```bash
ctest --test-dir build --output-on-failure
```

## 10. Demo Output
```text
=================================================================
        C PLUS PLUS CODE EDITOR (SYNTAX HIGHLIGHTER)             
=================================================================
[Test-Run Mode] Initializing buffer and syntax highlighting C++ snippet...

Buffer lines: 15 | Total characters: 395

--- Highlighted Output ---
  1 | #include <iostream>
  2 | #include <string>
  3 | 
  4 | // High-performance C Plus Plus service model
  5 | class OrderService : public BaseService {
  6 | private:
  7 |     const int maxRetries = 3;
  8 |     double timeoutSeconds = 15.5;
  9 | 
 10 | public:
 11 |     bool processOrder(const std::string& orderId) {
 12 |         std::cout << "Processing order: " << orderId << std::endl;
 13 |         return true;
 14 |     }
 15 | };
--------------------------
[Test-Run Mode] Executed successfully.
```

## 11. Known Limitations
- Highlighting is optimized for single-line tokens and standard preprocessor syntax.
- Full AST-based semantic analysis is out of scope for this lightweight editor component.

## 12. Testing Status
- **Built**: Yes, compiled cleanly with GCC 16.1.0 with 0 warnings.
- **Run**: Verified via `./build/CppCodeEditor --test-run`.
- **Tested**: 2 multi-case test suites passing under CTest.
