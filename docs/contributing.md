# Contributing Guide

Thank you for contributing to the **60 C++ Projects — Beginner to Advanced** repository! This document outlines guidelines for contributing new features, bug fixes, performance improvements, and documentation enhancements.

---

## 1. Hard Architectural Rules

1. **Language Standard**: C++ ONLY. C++17 by default; C++20 where justified.
2. **Independence**: Every project folder must remain completely independent. Never add `#include` directives or CMake linkages referencing another project.
3. **No Secrets**: Never commit passwords, tokens, API keys, or personal credentials. Use environment variables and provide `.env.example`.
4. **Build & Test Verification**: Any code submitted must compile cleanly without warnings (`-Wall -Wextra -Wpedantic` on GCC/Clang, `/W4` on MSVC) and pass all associated unit tests.
5. **C Plus Plus Only**: Every project in the repository is built 100% in C Plus Plus. All applications provide native C++ code editing, compilation driving, and systems capabilities.

---

## 2. Directory & Project Structure

Each project folder must maintain the canonical structure:
```
Project-Name/
├── src/            # Implementation (.cpp) files
├── include/        # Header (.h) files
├── tests/          # Unit tests (.cpp)
├── CMakeLists.txt  # Standalone build definition
├── README.md       # Comprehensive documentation
└── .gitignore      # Local ignores (build/, etc.)
```

---

## 3. Coding Style

- Adhere strictly to [docs/coding-standards.md](coding-standards.md).
- Format all source files using `.clang-format`:
  ```bash
  clang-format -i src/*.cpp include/*.h tests/*.cpp
  ```
- Adhere to RAII and prefer smart pointers (`std::unique_ptr`, `std::shared_ptr`) over raw `new`/`delete`.
- Avoid `using namespace std;` in any header.
- Enforce `const` correctness on parameters, methods, and variables.

---

## 4. Updating Documentation & Progress

When modifying an existing project or adding tests:
1. Update the project's local `README.md` to reflect new features, test coverage, and limitations.
2. Update the repository-level `PROGRESS.md` to record the status accurately (`Implemented`, `Builds`, `Tested`).
