# 18-Mad-Libs-Generator

An interactive template string parser and Mad Libs word substitution game engine built with modern C++17.

---

## 1. Project Overview
Mad Libs Generator parses text templates with bracketed placeholders (`<noun>`, `<verb>`, `<adjective>`), prompts users for substitutions, replaces occurrences safely, and exports completed humorous stories.

## 2. Problem Statement
String template rendering engines must accurately parse variable tags, handle missing or partial substitution keys without crashing, prevent buffer overflows, and support custom file-based story templates.

## 3. Objectives
- Tokenize and extract placeholders enclosed within `<...>` brackets.
- Substitute dictionary replacement maps into raw story templates.
- Preserve untruncated content for missing or partially filled tokens.
- Support both interactive CLI play and scripted key-value substitution.

## 4. Features
- **Flexible Grammar Placeholders**: Extracts any `<placeholder>` tag automatically.
- **Default Story Template**: Curated programming adventure template in `resources/story.txt`.
- **Scriptable CLI**: Non-interactive automated execution via `--inputs "k1=v1,k2=v2"`.
- **Disk Export**: Save completed stories to disk via `--export <path>`.

## 5. Technology Stack
- **Language**: C++17 (`std::map`, `std::vector`, `std::string`, `std::fstream`)
- **Build System**: CMake 3.20+
- **Testing Framework**: doctest with CTest
- **Database Needed**: No. Plaintext template storage.
- **Internet / External Service Needed**: No.

## 6. Folder Structure
```
18-Mad-Libs-Generator/
├── include/
│   └── MadLibsGenerator.h       # MadLibsEngine class API
├── src/
│   ├── MadLibsGenerator.cpp     # Parsing and string replacement algorithms
│   └── main.cpp                 # CLI driver and interactive prompt
├── resources/
│   └── story.txt                # Default story template
├── tests/
│   └── test_mad_libs.cpp        # doctest unit test suite
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

# Automated substitution execution
.\build\MadLibsGenerator.exe --inputs "adjective=sunny,programmer_name=Ada,project_type=compiler,beverage=coffee,hardware_item=keyboard,bug_type=memory,line_number=42,debugging_technique=GDB,speed_adjective=blazing,best_practice=RAII"
```

### Linux / macOS
```bash
cmake -S . -B build -G Ninja
cmake --build build

./build/MadLibsGenerator --inputs "adjective=gloomy,programmer_name=Linus,project_type=kernel,beverage=tea,hardware_item=laptop,bug_type=pointer,line_number=101,debugging_technique=Valgrind,speed_adjective=lightning,best_practice=UnitTests"
```

## 9. Testing Instructions
Run tests with CTest:
```bash
ctest --test-dir build --output-on-failure
```

## 10. Demo / Console Output Sample
```
$ ./build/MadLibsGenerator --inputs "adjective=sunny,programmer_name=Ada,project_type=compiler,beverage=coffee,hardware_item=keyboard,bug_type=memory,line_number=42,debugging_technique=GDB,speed_adjective=blazing,best_practice=RAII"

====================================================
                 YOUR COMPLETED STORY               
====================================================
It was a sunny morning when Ada decided to build a compiler in modern C++.
Equipped with a trusty cup of coffee and an ergonomic keyboard, they began writing code.
Suddenly, a wild memory bug appeared in line 42!
Without hesitating, they invoked their secret superpower: GDB.
The program compiled with zero warnings and executed at blazing speeds.
The moral of the story: always use RAII!
====================================================
```

## 11. Known Limitations
- Placeholders must not be nested (`<<tag>>` is undefined).
- Assumes ASCII tag characters.

## 12. Testing Status
- **Built**: Yes, compiled cleanly with GCC 16.1.0 using `-Wall -Wextra -Wpedantic`.
- **Run**: Yes, verified both interactive and automated CLI runs.
- **Tested**: Yes, 5 unit test suites passing via CTest.
- **Hardware/Credentials Limitations**: None.
