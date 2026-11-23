# 16-BMI-Calculator

A Body Mass Index (BMI) calculator and health categorization CLI utility adhering to World Health Organization (WHO) standards built with modern C++17.

---

## 1. Project Overview
BMI Calculator accepts user physical metrics in either Metric or Imperial units, calculates BMI values, maps results to WHO health risk categories, and provides healthy ideal target weight ranges.

## 2. Problem Statement
Health index calculators must handle both metric and imperial measurement paradigms, guard against physically impossible or hazardous values (e.g. division by zero or negative heights), and present medical risk categories accurately.

## 3. Objectives
- Compute Body Mass Index via both metric (\(\text{kg}/\text{m}^2\)) and imperial formulas.
- Classify BMI into WHO categories (Severe Thinness, Normal, Overweight, Obese Classes I-III).
- Calculate healthy target weight ranges corresponding to BMI \(18.5 - 24.9\).
- Defensively validate and reject implausible inputs.

## 4. Features
- **Dual Unit Support**: Metric (kg / meters / cm) and Imperial (lbs / inches).
- **WHO Categorization**: All 8 international medical classification tiers.
- **Ideal Weight Calculator**: Recommends lower and upper target weight boundaries.
- **Automated CLI**: Direct execution via `--metric` or `--imperial` flags.

## 5. Technology Stack
- **Language**: C++17 (`std::optional`, `std::pair`, `std::string`, `<cmath>`)
- **Build System**: CMake 3.20+
- **Testing Framework**: doctest with CTest
- **Database Needed**: No.
- **Internet / External Service Needed**: No.

## 6. Folder Structure
```
16-BMI-Calculator/
├── include/
│   └── BmiCalculator.h          # Classification enums and API
├── src/
│   ├── BmiCalculator.cpp        # Formula implementation and validation
│   └── main.cpp                 # CLI driver and report formatter
├── tests/
│   └── test_bmi_calculator.cpp  # doctest unit test suite
├── CMakeLists.txt               # CMake build configuration
├── .gitignore                   # Build ignore rules
└── README.md                    # Project documentation
```

## 7. Installation / Prerequisites
- GCC 11+, Clang 14+, or MSVC 2022
- CMake 3.20+ and Ninja (or Make)

## 8. Build and Run Instructions

### Windows (PowerShell)
```powershell
cmake -S . -B build -G Ninja
cmake --build build

# Metric mode (70kg, 1.75m)
.\build\BmiCalculator.exe --metric --weight 70.0 --height 1.75

# Imperial mode (160lbs, 68in)
.\build\BmiCalculator.exe --imperial --weight 160.0 --height 68.0
```

### Linux / macOS
```bash
cmake -S . -B build -G Ninja
cmake --build build

./build/BmiCalculator --metric --weight 70.0 --height 1.75
```

## 9. Testing Instructions
Run tests with CTest:
```bash
ctest --test-dir build --output-on-failure
```

## 10. Demo / Console Output Sample
```
$ ./build/BmiCalculator --metric --weight 70.0 --height 1.75

============================================
            BODY MASS INDEX REPORT          
============================================
BMI Score       : 22.9 kg/m²
Category        : Normal weight (Healthy)
Health Risk     : Minimal / Healthy baseline risk
--------------------------------------------
Healthy Range   : 56.7 - 76.3 kg (124.9 - 168.1 lbs)
============================================
```

## 11. Known Limitations
- BMI does not differentiate between body fat percentage and muscular hypertrophy (standard limitation of BMI index).
- Intended for adult assessments (pediatric BMI curves use age-based percentiles).

## 12. Testing Status
- **Built**: Yes, compiled cleanly with GCC 16.1.0 using `-Wall -Wextra -Wpedantic`.
- **Run**: Yes, verified both metric and imperial execution.
- **Tested**: Yes, 5 unit test suites passing via CTest.
- **Hardware/Credentials Limitations**: None.
