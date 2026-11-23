# 06-Custom-IDE-Plugin

## 1. Project Overview
06-Custom-IDE-Plugin is a cross-platform dynamic library plugin architecture designed for extensible integrated development environments (IDEs). Written in standard C++17, it decouples the core host editor from third-party tools, providing dynamic shared library loading (`.dll` on Windows, `.so` on POSIX), lifecycle control (`initialize`, `shutdown`), and command dispatching.

## 2. Problem Statement
Monolithic applications suffer from slow recompilation and lack of extensibility. Modern development environments require modular plugin architectures that allow developers to author, distribute, load, and unload modules at runtime without stopping or recompiling the host application.

## 3. Objectives
- Implement dynamic library loading abstractions using `LoadLibraryA`/`GetProcAddress`/`FreeLibrary` on Windows and `dlopen`/`dlsym`/`dlclose` on Linux.
- Define a stable C++ ABI interface (`IPlugin`, `IPluginContext`) and exported C factory/destroy entry points (`createPlugin`, `destroyPlugin`).
- Implement an automated code indentation and formatting sample plugin (`FormatterPlugin`).
- Provide automated verification with test suites running under CTest.

## 4. Features
- **Dynamic Module Management**: Load and unload shared libraries dynamically with RAII lifetime management.
- **Bi-Directional Host Interaction**: Plugins interact with the host via `IPluginContext` to register commands and emit structured diagnostic logs.
- **Isolated Plugin Scope**: Decoupled memory allocation through dedicated export pairs (`createPlugin`/`destroyPlugin`).
- **Sample Formatter Plugin**: Built-in syntax aware indentation and bracket alignment plugin.
- **Robust Error Resilience**: Safe fallback handling when encountering invalid files or missing symbols.

## 5. Technology Stack
- **Language**: C++17 (`std::unique_ptr`, `std::vector`, `std::map`, `std::filesystem`).
- **Dynamic Linking APIs**: Win32 Dynamic-Link Library API (`LoadLibraryA`) / POSIX `libdl`.
- **External Services**: None. Fully self-contained.
- **Database**: None required.
- **Testing**: `doctest` unit testing framework.

## 6. Folder Structure
```
06-Custom-IDE-Plugin/
├── CMakeLists.txt
├── README.md
├── .gitignore
├── include/
│   ├── DynamicLibrary.hpp
│   ├── IPlugin.hpp
│   └── PluginManager.hpp
├── plugins/
│   └── FormatterPlugin.cpp
├── src/
│   ├── PluginManager.cpp
│   └── main.cpp
└── tests/
    └── test_plugin_system.cpp
```

## 7. Installation / Prerequisites
- CMake 3.20 or newer.
- GCC 11+, Clang 14+, or MSVC 2022.
- doctest headers.

## 8. Build and Run Instructions
### Windows & Linux Build
```bash
cmake -S . -B build -G Ninja
cmake --build build
```

### Running the Host Engine
```bash
./build/CustomIDEHost --plugin build/plugins/libFormatterPlugin.dll
```

### Automated Verification Mode
```bash
./build/CustomIDEHost --test-run
```

## 9. Testing Instructions
Run CTest:
```bash
ctest --test-dir build --output-on-failure
```

## 10. Console Output Sample
```
=== Minimal IDE Plugin Host Architecture ===
Found plugin at: plugins/libFormatterPlugin.dll
[HostContext] [INFO] CppFormatter initialized successfully.
[HostContext] Registered command: 'format' -> Indents and strips trailing whitespaces.
[HostContext] [INFO] Successfully loaded plugin: CppFormatter v1.0.0
Loaded Plugins Count: 1

Dispatching 'format' command to CppFormatter:
--- Formatted Output ---
int main() {
    int x = 10;
    if (x > 5) {
        return 0;
    }
}
------------------------
[Test Mode] Dynamic plugin host verification executed successfully.
```

## 11. Known Limitations
- C++ interfaces between host and plugins require matching ABI and compiler runtime versions.
- Dynamic plugins execute in-process without memory sandbox isolation.

## 12. Testing Status
- **Built**: Yes, with GCC 16.1.0 on Windows with 0 warnings (`-Wall -Wextra -Wpedantic`).
- **Run**: Verified host startup, dynamic DLL load, command dispatch, and formatted output generation.
- **Tested**: All unit test suites passed under CTest.
