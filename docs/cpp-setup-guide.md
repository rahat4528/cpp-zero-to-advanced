# C++ Setup Guide: Compilers, Toolchains, and Build Systems

This guide explains how to install and configure the necessary C++ compilers, CMake, and build tools across Windows, Linux, and macOS.

---

## 1. Prerequisites Overview

Each project in this repository targets **C++17** (or **C++20** where modern features are explicitly needed). To build and run these projects, you need:

- **C++ Compiler**:
  - GCC 11+ or
  - Clang 14+ or
  - MSVC 2022 (Visual Studio 17.0+)
- **Build System**:
  - CMake 3.20 or newer
  - Build generator: Ninja (recommended for fast builds) or Make / MSBuild
- **Version Control**: Git 2.30+

---

## 2. Windows Setup

### Option A: MinGW-w64 via MSYS2 or WinGet (Recommended)
1. **Install MinGW-w64 GCC**:
   - Download and install MSYS2 from [msys2.org](https://www.msys2.org/), or install GCC via your package manager.
   - In MSYS2 terminal:
     ```bash
     pacman -Syu
     pacman -S mingw-w64-x86_64-gcc mingw-w64-x86_64-gdb mingw-w64-x86_64-ninja
     ```
   - Add `C:\msys64\mingw64\bin` to your system `PATH`.

2. **Install CMake and Ninja**:
   - Using Windows Package Manager (`winget` in PowerShell):
     ```powershell
     winget install --id Kitware.CMake -e
     winget install --id Ninja-build.Ninja -e
     ```
   - Ensure CMake and Ninja are added to your `PATH`.

3. **Verify Installation**:
   ```powershell
   g++ --version
   cmake --version
   ninja --version
   ```

### Option B: Visual Studio 2022 (MSVC)
1. Download **Visual Studio 2022 Community Edition** from [visualstudio.microsoft.com](https://visualstudio.microsoft.com/).
2. During installation, select the workload: **Desktop development with C++**.
3. Under installation details, ensure the following optional components are checked:
   - MSVC v143 - VS 2022 C++ x64/x86 build tools
   - C++ CMake tools for Windows
   - C++ AddressSanitizer
4. Use the **x64 Native Tools Command Prompt for VS 2022** or configure PowerShell with the MSVC environment.

---

## 3. Linux Setup (Ubuntu / Debian)

1. **Install build tools and compiler**:
   ```bash
   sudo apt update
   sudo apt install -y build-essential g++ gdb git ninja-build
   ```

2. **Install CMake 3.20+**:
   ```bash
   sudo apt install -y cmake
   # Verify version
   cmake --version
   ```
   If the distro repository provides CMake < 3.20, install the official Kitware APT repository or download precompiled binaries from [cmake.org](https://cmake.org/download/).

3. **Verify Installation**:
   ```bash
   g++ --version
   cmake --version
   ninja --version
   ```

---

## 4. macOS Setup

1. **Install Xcode Command Line Tools**:
   ```bash
   xcode-select --install
   ```

2. **Install modern GCC or Clang and CMake via Homebrew**:
   ```bash
   brew install cmake ninja gcc
   ```

3. **Verify Installation**:
   ```bash
   clang++ --version
   cmake --version
   ninja --version
   ```

---

## 5. Verifying Your Toolchain with a Test Build

You can quickly verify that your toolchain can configure and compile a modern C++17 binary:

```bash
mkdir toolchain_test && cd toolchain_test
cat << 'EOF' > main.cpp
#include <iostream>
#include <string_view>

int main() {
    constexpr std::string_view message = "C++17 Toolchain is functional!";
    std::cout << message << '\n';
    return 0;
}
EOF

cat << 'EOF' > CMakeLists.txt
cmake_minimum_required(VERSION 3.20)
project(ToolchainTest CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
add_executable(toolchain_test main.cpp)
EOF

cmake -S . -B build -G Ninja
cmake --build build
./build/toolchain_test
```
On Windows, run `.\build\toolchain_test.exe`. If you see `C++17 Toolchain is functional!`, your environment is fully ready.
