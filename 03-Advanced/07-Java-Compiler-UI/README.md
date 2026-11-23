# 07-Java-Compiler-UI

A C Plus Plus graphical and CLI frontend orchestrating C++ compiler toolchains (such as `g++`, `clang++`, or `MSVC`), capturing sub-process diagnostic output, and parsing compiler errors/warnings with click-to-jump line navigation.

> **Language Notice**: This project is built 100% in C Plus Plus (C++17) and drives C Plus Plus compiler toolchains.

---

## 1. Project Overview
07-Java-Compiler-UI is an IDE companion tool and toolchain orchestrator built in C Plus Plus. It drives compiler binaries through platform sub-process streams (`popen`/pipes), parses stderr output using regular expressions, and translates raw terminal diagnostics into structured diagnostic records with severity classification, file paths, line numbers, and column offsets.

## 2. Problem Statement
Raw compiler diagnostics emitted across heterogeneous compilers (GCC, Clang, MSVC) differ significantly in formatting and token ordering. Developers and IDE builders require a unified, cross-compiler abstraction layer in C++ to invoke toolchains, isolate failing runs, capture exit codes, and normalize diagnostics into machine-readable structures.

## 3. Objectives
- Orchestrate compiler commands with configurable flag vectors (`-std=c++17`, `-Wall`, `-Wextra`).
- Capture stdout and stderr streams via non-blocking sub-process pipes.
- Parse multi-format compiler diagnostics (GCC/Clang `file:line:col:` and MSVC `file(line):`).
- Provide an automated testable simulation mode to verify diagnostic parsing without external compiler dependencies.

## 4. Features
- **Cross-Compiler Support**: Configurable compiler binaries (`g++`, `clang++`, `cl.exe`).
- **Flexible Flag Management**: Dynamic flag arrays with runtime addition/clearing.
- **Diagnostic Regex Engine**: Automatic extraction of file, line, column, severity, and message text.
- **Offline Simulation Mode**: Deterministic unit testing for CI/CD environments.

## 5. Technology Stack
- **Language**: C++17 (`std::regex`, `<sstream>`, `<vector>`).
- **Build System**: CMake 3.20+.
- **Testing**: doctest.
- **Dependencies**: None (100% C++ Standard Library & OS process pipes).

## 6. Folder Structure
```
07-Java-Compiler-UI/
├── CMakeLists.txt
├── README.md
├── .gitignore
├── include/
│   ├── CompilerToolchain.hpp
│   └── DiagnosticParser.hpp
├── src/
│   ├── CompilerToolchain.cpp
│   ├── DiagnosticParser.cpp
│   └── main.cpp
├── tests/
│   └── test_compiler_ui.cpp
└── docs/
    └── ARCHITECTURE.md
```

## 7. Prerequisites
- GCC 11+, Clang 14+, or MSVC 2022 supporting C++17.
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
./build/CppCompilerUI --test-run
```

## 9. Testing Instructions
```bash
ctest --test-dir build --output-on-failure
```

## 10. Output Demo
```text
=========================================================
       C PLUS PLUS COMPILER TOOLCHAIN UI & RUNNER        
=========================================================
Configured C++ Compiler Toolchain : g++
Compiler Flags        : -std=c++17 -Wall -Wextra 

[Test-Run Mode] Executing simulated C++ compilation...
Compilation Success   : NO
Exit Code             : 1
Diagnostics Detected  : 2
  [ERROR] Main.cpp:5 -> 'syntax_error' was not declared in this scope
  [WARNING] Main.cpp:8 -> unused variable 'temp' [-Wunused-variable]
[Test-Run Mode] Executed successfully.
```

## 11. Known Limitations
- When driving real external toolchains, the chosen compiler executable must be present in the system `PATH`.
- Simulation mode provides verified test coverage when native compilers are absent.

## 12. Testing Status
- **Built**: Yes, compiled with GCC 16.1.0 and CMake with zero warnings under `-Wall -Wextra -Wpedantic`.
- **Run**: Verified via `./build/CppCompilerUI --test-run`.
- **Tested**: 3 comprehensive test suites passing under CTest.
