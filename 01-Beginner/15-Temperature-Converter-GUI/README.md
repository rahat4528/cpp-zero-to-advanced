# 15-Temperature-Converter-GUI

A visual thermodynamic Temperature Conversion and Gauge UI utility built with modern C++17.

---

## 1. Project Overview
Temperature Converter GUI / TUI converts temperatures between Celsius, Fahrenheit, Kelvin, and Rankine scales. It enforces physical laws at absolute zero (\(-273.15^\circ\text{C}\)), provides thermodynamic phase state descriptions, and renders interactive ASCII thermometer gauges.

## 2. Problem Statement
Thermodynamic calculations require strict boundary checks (sub-absolute-zero temperatures are physically impossible). Applications require bidirectional conversion between industrial, scientific, and metric scales paired with clean visual representations.

## 3. Objectives
- Convert interchangeably across Celsius, Fahrenheit, Kelvin, and Rankine scales.
- Enforce thermodynamic absolute zero barriers (\(0\text{ K} / -273.15^\circ\text{C}\)).
- Provide temperature phase classifications (Freezing, Cool, Room Temp, Boiling).
- Render visual ASCII thermometer meters.

## 4. Features
- **4 Temperature Scales**: Celsius, Fahrenheit, Kelvin, Rankine.
- **Thermodynamic Guardrails**: Blocks values below absolute zero.
- **Thermometer Gauge Visualizer**: Dynamically draws a 20-segment scale meter with marker indicator.
- **CLI Automation**: Scriptable via `--val <temp> --scale <c|f|k|r>`.

## 5. Technology Stack
- **Language**: C++17 (`std::optional`, `std::string`, `std::ostringstream`, `<cmath>`)
- **UI Toolkit**: Visual Terminal UI / ASCII Gauge Meter. *Rationale*: Selected over heavy Qt6 dependencies to maintain instant compilation and cross-platform zero-dependency execution across Windows and Linux.
- **Build System**: CMake 3.20+
- **Testing Framework**: doctest with CTest
- **Database Needed**: No.
- **Internet / External Service Needed**: No.

## 6. Folder Structure
```
15-Temperature-Converter-GUI/
├── include/
│   └── TemperatureConverter.h       # Scale enum and converter declarations
├── src/
│   ├── TemperatureConverter.cpp     # Conversion algorithms and gauge renderer
│   └── main.cpp                     # CLI driver and report formatter
├── tests/
│   └── test_temperature_converter.cpp # doctest unit test suite
├── CMakeLists.txt                   # CMake build configuration
├── .gitignore                       # Build ignore rules
└── README.md                        # Project documentation
```

## 7. Installation / Prerequisites
- GCC 11+, Clang 14+, or MSVC 2022
- CMake 3.20+ and Ninja (or Make)

## 8. Build and Run Instructions

### Windows (PowerShell)
```powershell
cmake -S . -B build -G Ninja
cmake --build build

# Convert Celsius to all scales
.\build\TemperatureConverter.exe --val 100 --scale c

# Convert Fahrenheit to all scales
.\build\TemperatureConverter.exe --val 32 --scale f
```

### Linux / macOS
```bash
cmake -S . -B build -G Ninja
cmake --build build

./build/TemperatureConverter --val 22.5 --scale c
```

## 9. Testing Instructions
Run tests with CTest:
```bash
ctest --test-dir build --output-on-failure
```

## 10. Demo / Console Output Sample
```
$ ./build/TemperatureConverter --val 100 --scale c

====================================================
           TEMPERATURE CONVERSION REPORT            
====================================================
Visual Gauge : [=================|..] 100.0 °C
Status/Phase : Boiling Point or Higher
----------------------------------------------------
Celsius      : 100.00 °C
Fahrenheit   : 212.00 °F
Kelvin       : 373.15 K
Rankine      : 671.67 °R
====================================================
```

## 11. Known Limitations
- Rankine conversions rounded to 2 decimal places in console presentation.
- Negative temperatures below -20°C clamp visually to the lower end of the gauge meter.

## 12. Testing Status
- **Built**: Yes, compiled cleanly with GCC 16.1.0 using `-Wall -Wextra -Wpedantic`.
- **Run**: Yes, verified multiple conversions and absolute zero boundaries.
- **Tested**: Yes, 4 test suites passing via CTest.
- **Hardware/Credentials Limitations**: None.
