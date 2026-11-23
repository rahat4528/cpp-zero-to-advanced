# 03-Unit-Converter

A multi-category scientific and dimensional Unit Converter built with modern C++17.

---

## 1. Project Overview
Unit Converter is a command-line conversion utility supporting length, mass, temperature, and speed conversions. It enforces physical laws such as absolute zero bounds and positive length/mass limits.

## 2. Problem Statement
Unit conversion errors in engineering and science cause costly discrepancies. Developers require an extensible, verified converter that validates dimensions, normalizes units to baseline SI scales, and handles edge cases reliably.

## 3. Objectives
- Implement category-based conversions for Length, Mass, Temperature, and Speed.
- Normalize measurements against base SI units (meters, grams, Celsius, meters/second).
- Enforce strict physics constraints (e.g. rejection of temperatures below \(0\text{ K}\) or \(-273.15^\circ\text{C}\)).
- Provide dual-mode CLI operation (argument flags and interactive REPL).

## 4. Features
- **Length**: Meters, Kilometers, Centimeters, Millimeters, Miles, Yards, Feet, Inches.
- **Mass**: Kilograms, Grams, Milligrams, Pounds, Ounces, Metric Tonnes.
- **Temperature**: Celsius, Fahrenheit, Kelvin with thermodynamic boundary enforcement.
- **Speed**: Meters/second, Kilometers/hour, Miles/hour, Knots.
- **Precision Output**: Formatted fixed-point scientific output.

## 5. Technology Stack
- **Language**: C++17 (`std::unordered_map`, `std::optional`, `std::string`, `std::vector`)
- **Build System**: CMake 3.20+
- **Testing Framework**: doctest with CTest
- **Database Needed**: No. In-memory lookups.
- **Internet / External Service Needed**: No.

## 6. Folder Structure
```
03-Unit-Converter/
├── include/
│   └── UnitConverter.h       # Category declarations and conversion API
├── src/
│   ├── UnitConverter.cpp     # Conversion algorithms and SI mappings
│   └── main.cpp              # CLI parser and interactive prompt
├── tests/
│   └── test_unit_converter.cpp # doctest unit test suite
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

# CLI mode:
.\build\UnitConverter.exe --cat length --from km --to mi --val 10
.\build\UnitConverter.exe --cat temp --from c --to f --val 100

# List supported units:
.\build\UnitConverter.exe --list
```

### Linux / macOS
```bash
cmake -S . -B build -G Ninja
cmake --build build

./build/UnitConverter --cat length --from km --to mi --val 10
```

## 9. Testing Instructions
Run unit tests with CTest:
```bash
ctest --test-dir build --output-on-failure
```

## 10. Demo / Console Output Sample
```
$ ./build/UnitConverter --cat length --from km --to mi --val 10
10.0000 km = 6.2137 mi

$ ./build/UnitConverter --cat temp --from c --to f --val 100
100.0000 c = 212.0000 f
```

## 11. Known Limitations
- Does not currently support compound units (e.g. \(\text{kg}\cdot\text{m}/\text{s}^2\)).
- Fixed-point precision formatted to 4 decimal places by default.

## 12. Testing Status
- **Built**: Yes, compiled with GCC 16.1.0 using `-Wall -Wextra -Wpedantic`.
- **Run**: Yes, verified multiple category conversions via CLI flags.
- **Tested**: Yes, 5 comprehensive test suites passing with 15 assertions.
- **Hardware/Credentials Limitations**: None.
