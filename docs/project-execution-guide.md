# Project Execution & Testing Guide

Every project in this repository is designed to be **completely self-contained and independent**. There are no shared build targets or code dependencies between projects.

---

## 1. Standard Build Pattern

Each individual project folder contains its own `CMakeLists.txt`. To build any project:

### Step 1: Navigate to the Project Directory
```bash
# Example: navigating to Beginner project 01
cd 01-Beginner/01-Grade-Calculator
```

### Step 2: Configure the Build with CMake
```bash
# Using Ninja (recommended)
cmake -S . -B build -G Ninja

# Or using default system generator
cmake -S . -B build
```

Options:
- `-DCMAKE_BUILD_TYPE=Release` for optimized binaries
- `-DCMAKE_BUILD_TYPE=Debug` for debug builds with symbols
- `-DBUILD_TESTING=ON` (enabled by default) to build the test suite

### Step 3: Compile
```bash
cmake --build build
```
Or specify parallelism:
```bash
cmake --build build --parallel
```

---

## 2. Running the Application

After building, executable binaries are located in the `build/` directory (or `build/bin/` depending on generator configuration).

### Windows (PowerShell or Command Prompt)
```powershell
.\build\GradeCalculator.exe
```

### Linux / macOS
```bash
./build/GradeCalculator
```

### Command-line Arguments and Interactive Modes
Many projects provide both interactive CLI modes and command-line argument flags (e.g. `--help`, `--file <path>`, `--test`). Run with `--help` to inspect supported arguments:
```bash
.\build\GradeCalculator.exe --help
```

---

## 3. Running Unit Tests with CTest

Every project with testable business logic defines a test target hooked into CMake's testing harness.

### Run All Tests via CTest:
```bash
ctest --test-dir build --output-on-failure
```

### Direct Test Executable Execution:
You can also run the test executable directly to pass doctest or Catch2 CLI options (such as filtering test cases or verbosity):
```powershell
# Windows
.\build\GradeCalculator_tests.exe

# Filter specific test cases:
.\build\GradeCalculator_tests.exe -tc="*average*"
```

---

## 4. Cleaning Build Artifacts

To completely clean a project build:
```bash
# Delete the build folder
cmake --build build --target clean
# Or remove the directory directly:
rm -rf build        # Linux/macOS
Remove-Item -Recurse -Force build  # Windows PowerShell
```
Since `build/` is included in the root `.gitignore`, build artifacts will never pollute git history.
