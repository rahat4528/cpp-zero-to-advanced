# Modern C++ Coding Standards & Best Practices

This document establishes the coding conventions, idioms, and design rules for all projects in this repository. All code targets **C++17** by default (and **C++20** where modern features such as concepts or ranges provide significant value).

---

## 1. Core Principles

1. **RAII (Resource Acquisition Is Initialization)**:
   - Encapsulate resource acquisition (memory, file handles, sockets, database connections, mutex locks) in constructors.
   - Release resources automatically in destructors.
   - Never manage manual resources using raw `new` and `delete`.

2. **Smart Pointers Over Raw Pointers**:
   - Prefer value semantics on the stack when possible.
   - Use `std::unique_ptr` for exclusive heap ownership (`std::make_unique<T>()`).
   - Use `std::shared_ptr` and `std::weak_ptr` only when shared ownership is genuinely required (`std::make_shared<T>()`).
   - Raw pointers (`T*`) are strictly non-owning observers that must never be `delete`d. Prefer references (`const T&` or `T&`) over raw pointers when an object cannot be null.

3. **Const Correctness**:
   - Mark member functions that do not alter object state as `const`.
   - Pass read-only parameters by `const&` (or by value for cheap-to-copy types like fundamental types or `std::string_view`).
   - Declare local variables `const` whenever they do not need modification after initialization.

4. **Rule of Zero / Three / Five**:
   - Prefer the **Rule of Zero**: let the compiler generate the destructor, copy, and move operations by utilizing STL containers and smart pointers.
   - If a custom destructor, copy constructor, copy assignment operator, move constructor, or move assignment operator is needed, explicitly declare or delete all five (**Rule of Five**).

5. **Namespace Hygiene**:
   - **Never** place `using namespace std;` in header files (`.h` or `.hpp`).
   - In implementation files (`.cpp`), prefer scoped `using` declarations (e.g. `using std::string;`) or qualify with `std::` to prevent naming collisions.

---

## 2. Naming Conventions

| Entity | Convention | Example |
| :--- | :--- | :--- |
| **Classes, Structs, Enums** | `PascalCase` | `GradeCalculator`, `StudentRecord`, `HttpStatus` |
| **Functions, Methods** | `camelCase` (or `snake_case` consistently per project) | `calculateAverage()`, `loadFromFile()`, `isValid()` |
| **Local Variables** | `snake_case` or `camelCase` | `student_count`, `bufferSize`, `totalScore` |
| **Member Variables** | Prefix with `m_` or trailing `_` | `m_name`, `m_balance` or `name_`, `balance_` |
| **Constants / Enums** | `kPascalCase` or `UPPER_SNAKE_CASE` | `kMaxRetries`, `DEFAULT_PORT`, `Status::Success` |
| **Namespaces** | lowercase / `snake_case` | `grade_calc`, `banking::core` |
| **Templates** | Single uppercase letter or `PascalCase` | `template <typename T>`, `template <typename Allocator>` |

---

## 3. Header and Include Conventions

1. Always use `#pragma once` at the beginning of header files.
2. Order `#include` directives logically:
   - Corresponding project header (e.g., `GradeCalculator.h` in `GradeCalculator.cpp`)
   - C++ Standard Library headers (`<iostream>`, `<vector>`, `<string>`, `<memory>`)
   - Third-party library headers (Catch2, doctest, SQLite, Curl, etc.)
   - Other project headers (`"utils/Logger.h"`)
3. Use angle brackets `<...>` for system and third-party headers; use double quotes `"..."` for local project headers.
4. Keep headers minimal; use forward declarations where full definitions are not needed to reduce build times.

---

## 4. Modern C++ Features & Idioms

- **Type Deduction**: Use `auto` when the type is repeated or obvious from the right-hand side (e.g., `auto ptr = std::make_unique<Widget>();`), or with complex iterator types. Avoid `auto` when it obscures vital type information.
- **String Handling**: Use `std::string_view` for read-only string parameters that do not require ownership.
- **Optional & Variant**: Prefer `std::optional<T>` for values that may or may not exist instead of sentinel values (like `-1` or `nullptr`). Use `std::variant` for type-safe tagged unions.
- **Range-based Loops**: Always prefer `for (const auto& item : collection)` or `for (auto&& item : collection)` over indexed loops unless the index is actively needed.
- **Null Safety**: Always use `nullptr`, never `0` or `NULL`.
- **Initialization**: Prefer uniform brace initialization `T x{...}` to avoid vexing parse bugs and prevent unintended narrowing conversions.

---

## 5. Error Handling & Safety

1. **Exceptions**:
   - Use standard exception classes (`std::runtime_error`, `std::invalid_argument`, `std::out_of_range`) or derive custom exceptions from `std::exception`.
   - Never throw raw primitives (e.g. `throw 404;` or `throw "error";`).
   - Destructors must never throw (`noexcept` by default).

2. **Error Codes / Expected Values**:
   - In performance-critical loops or non-exceptional cases (such as parsing user input), return structured error codes, `std::optional`, or a status object.

3. **Input Validation**:
   - Sanitize and validate all user input at the application boundary (CLI arguments, stdin, GUI text boxes, file data, network payloads).
   - Check array and vector bounds (`vec.at(i)` or explicit size checks before indexing).

---

## 6. Compiler Warnings & Formatting

- All projects must compile cleanly with `-Wall -Wextra -Wpedantic` (GCC/Clang) or `/W4` (MSVC).
- Treat critical warnings as errors during development.
- Adhere to the root `.clang-format` file for consistent indentation (4 spaces), bracket styles, and whitespace.
