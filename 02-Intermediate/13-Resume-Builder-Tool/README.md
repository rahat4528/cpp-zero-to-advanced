# Project 13: Resume-Builder-Tool

## 1. Project Overview
`Resume-Builder-Tool` is a structured document generation engine that compiles applicant career profiles (education, roles, key achievements, skills) into multi-target presentation formats: GitHub-Flavored Markdown, semantic responsive HTML5, and ANSI plaintext.

## 2. Problem Statement
Job seekers frequently need to customize, version-control, and compile their professional experience into different formats tailored for distinct distribution channels without manually formatting each document type.

## 3. Objectives
- Establish a normalized data model representing candidate details, skill tags, employment chronologies, and academic qualifications.
- Provide template synthesis routines emitting valid Markdown and clean semantic HTML5 with modern CSS styling.
- Ensure strict input validation for mandatory contact fields.
- Allow headless automated execution and file export verification.

## 4. Features
- **Multi-Format Compilation**: Emits Markdown (`.md`), styled HTML5 (`.html`), and formatted plain text.
- **Data Validation**: Checks for minimum required candidate contact attributes.
- **Modern HTML Design**: Responsive CSS stylesheet with category badges, typography hierarchy, and print-ready spacing.
- **Headless Pipeline**: Integrated `--test-run` flag to support test and artifact generation without interactive prompts.

## 5. Technology Stack
- **Language**: C++17
- **Compiler**: GCC 11+, Clang 14+, MSVC 2022
- **Build System**: CMake 3.20+
- **Testing**: doctest 2.4+ via CTest
- **External Services**: None (100% offline)

## 6. Folder Structure
```
13-Resume-Builder-Tool/
├── include/
│   └── ResumeModel.hpp
├── src/
│   ├── ResumeModel.cpp
│   └── main.cpp
├── tests/
│   └── test_resume_builder.cpp
├── CMakeLists.txt
├── README.md
└── .gitignore
```

## 7. Installation / Prerequisites
- CMake 3.20+
- C++17 compliant compiler
- Ninja or Make

## 8. Build and Run Instructions
### Windows & Linux
```bash
cmake -S . -B build -G Ninja
cmake --build build

# Run automated test generation:
./build/ResumeBuilderTool --test-run
```

## 9. Testing Instructions
```bash
ctest --test-dir build --output-on-failure
```

## 10. Console Output Sample
```
=================================================================
               RESUME BUILDER & TEMPLATE COMPILER                
=================================================================
[Test-Run Mode] Generating sample resume and rendering formats...
Successfully generated:
  - Markdown output (1240 bytes) -> sample_resume.md
  - HTML5 output    (2185 bytes) -> sample_resume.html
  - PlainText preview:

=================================================================
 Alex Mercer - Senior Systems Software Engineer
 Email: alex.mercer@systems.dev | Phone: +1 (555) 234-5678
=================================================================
```

## 11. Known Limitations
- Current engine utilizes built-in stringstream templates; dynamic Jinja2/Mustache template loading is planned for future iterations.

## 12. Testing Status
- **Built**: Yes (Clean compilation under GCC 16.1.0 on Windows, 0 warnings).
- **Run**: Verified `--test-run` and file generation.
- **Tested**: 2 multi-subcase test suites passing under CTest.
