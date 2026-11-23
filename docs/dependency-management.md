# Dependency Management Guide

This document explains how dependencies are managed across the repository, the design philosophy behind choosing external libraries, and how CMake FetchContent and vcpkg manifest mode are used.

---

## 1. Philosophy: Standard Library First

The guiding principle of this repository is to maximize learning by relying on the **C++ Standard Library (`std::`)** wherever feasible:
- Containers: `std::vector`, `std::map`, `std::unordered_map`, `std::array`, `std::span`
- Memory management: `std::unique_ptr`, `std::shared_ptr`
- Text processing: `std::string`, `std::string_view`, `std::regex`
- Files and streams: `std::filesystem`, `std::fstream`, `std::stringstream`
- Concurrency: `std::thread`, `std::mutex`, `std::atomic`, `std::condition_variable`, `std::async`
- Chrono & Random: `std::chrono`, `std::random_device`, `std::mt19937`

External libraries are introduced strictly when standard alternatives do not exist (e.g. SQLite database access, HTTP client networking with libcurl, image decoding with stb_image, cryptographic primitives with OpenSSL, or unit testing harnesses like doctest/Catch2).

---

## 2. CMake FetchContent

For lightweight and header-only dependencies, **CMake FetchContent** is the preferred delivery mechanism because it requires zero external tool installations and automatically downloads dependencies during CMake configuration.

### Best Practice Example (doctest):
```cmake
include(FetchContent)

set(CMAKE_POLICY_VERSION_MINIMUM 3.5 CACHE STRING "" FORCE)

FetchContent_Declare(
    doctest
    GIT_REPOSITORY https://github.com/doctest/doctest.git
    GIT_TAG v2.4.11
    GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(doctest)
```

### Advantages of FetchContent:
- Pin exact versions (tags or commit SHAs) for reproducible builds.
- Works cross-platform on Windows, Linux, and macOS without package manager setup.
- `GIT_SHALLOW TRUE` minimizes download bandwidth and time.

---

## 3. vcpkg Manifest Mode (`vcpkg.json`)

For heavier C/C++ libraries (such as SQLite3, libcurl, OpenSSL, or Qt6), projects provide a `vcpkg.json` manifest file.

### Example `vcpkg.json`:
```json
{
  "name": "sqlite-student-database",
  "version-string": "1.0.0",
  "dependencies": [
    "sqlite3",
    "doctest"
  ]
}
```

### Building with vcpkg:
```bash
cmake -S . -B build -DCMAKE_TOOLCHAIN_FILE=[path/to/vcpkg]/scripts/buildsystems/vcpkg.cmake
cmake --build build
```

---

## 4. Single-Header and Self-Contained Libraries

Certain utilities (such as `stb_image.h` for image loading or single-file SQLite amalgamation `sqlite3.c`/`sqlite3.h`) can also be vendored directly or fetched cleanly when minimal footprint is beneficial.

Whenever an external dependency is introduced, the project's `README.md` explicitly documents:
1. Why the library was chosen.
2. How CMake discovers or links it.
3. Fallback or mock mechanisms when hardware or credentials are required.
