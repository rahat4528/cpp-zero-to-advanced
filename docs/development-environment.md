# Development Environment Guide

This document outlines the recommended developer tooling, editor setups, debugger integrations, and formatting workflows for navigating and modifying projects in this repository.

---

## 1. Recommended Editors & IDEs

### A. Visual Studio Code
VS Code provides a fast, lightweight, and extensible development workflow across Windows, Linux, and macOS.

**Recommended Extensions:**
1. **C/C++** (`ms-vscode.cpptools`) - IntelliSense, debugging, and code browsing.
2. **CMake Tools** (`ms-vscode.cmake-tools`) - Full CMake configure, build, and CTest integration directly inside the status bar.
3. **Clang-Format** (`xaver.clang-format`) - Automatic code formatting matching the repo's `.clang-format`.
4. **GitLens** (`eamodio.gitlens`) - Code provenance and history visualization.

**Configuring VS Code Settings (`.vscode/settings.json`):**
```json
{
    "cmake.buildDirectory": "${workspaceFolder}/build",
    "cmake.generator": "Ninja",
    "editor.formatOnSave": true,
    "editor.defaultFormatter": "xaver.clang-format",
    "C_Cpp.default.cppStandard": "c++17",
    "C_Cpp.default.intelliSenseMode": "windows-gcc-x64"
}
```

### B. CLion (JetBrains)
CLion natively understands CMake projects:
1. Open the specific project directory (e.g. `01-Beginner/01-Grade-Calculator`).
2. CLion detects `CMakeLists.txt` automatically.
3. In **Settings -> Build, Execution, Deployment -> CMake**, choose **Ninja** as the generator and ensure the C++17 standard is configured.
4. Set Code Style to use `.clang-format`.

### C. Visual Studio 2022
1. Select **File -> Open -> Folder...** and point to the specific project directory.
2. Visual Studio's CMake integration parses `CMakeLists.txt` and generates the build cache.
3. Select the target from the startup dropdown (e.g. `GradeCalculator.exe`) and press `F5` to build and debug.

---

## 2. Code Formatting & Linting

### Using clang-format
A root `.clang-format` is provided in the repository adhering to modern, clean C++ styling:
- 4-space indentation, no tabs
- 100-character line width
- Braces attached for functions and control blocks
- Clustered and sorted includes

To format a file manually from the terminal:
```bash
clang-format -i src/*.cpp include/*.h tests/*.cpp
```

---

## 3. Debugging Workflows

### Debugging with GDB (MinGW / Linux)
1. Build with debug symbols:
   ```bash
   cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug -G Ninja
   cmake --build build
   ```
2. Launch GDB:
   ```bash
   gdb ./build/GradeCalculator
   ```
3. Set breakpoints and run:
   ```gdb
   (gdb) break main
   (gdb) run
   (gdb) next
   (gdb) print my_variable
   (gdb) continue
   ```

### Debugging with VS Code
A sample `.vscode/launch.json`:
```json
{
    "version": "0.2.0",
    "configurations": [
        {
            "name": "Debug Executable",
            "type": "cppdbg",
            "request": "launch",
            "program": "${command:cmake.launchTargetPath}",
            "args": [],
            "stopAtEntry": false,
            "cwd": "${workspaceFolder}",
            "environment": [],
            "externalConsole": false,
            "MIMode": "gdb",
            "setupCommands": [
                {
                    "description": "Enable pretty-printing for gdb",
                    "text": "-enable-pretty-printing",
                    "ignoreFailures": true
                }
            ]
        }
    ]
}
```
