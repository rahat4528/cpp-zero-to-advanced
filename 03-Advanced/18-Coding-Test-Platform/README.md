# 18-Coding-Test-Platform

## 1. Project Overview
18-Coding-Test-Platform is an automated competitive programming judge system and code execution sandbox implemented in modern C++17. Designed to automate technical assessments (similar to LeetCode, Codeforces, and HackerRank), it compiles submitted C++ source files, runs programs against input test cases with real-time process monitoring, enforces strict CPU Time Limits (TL) and Memory Limits (ML), evaluates output equivalence, and outputs verdicts (`AC`, `WA`, `TLE`, `MLE`, `RE`, `CE`).

## 2. Problem Statement
Evaluating untrusted user code submissions automatically requires precise resource bounding: infinite loops must be terminated before blocking host threads, memory leaks must not exhaust host RAM, and runtime crashes must be isolated from the evaluation platform.

## 3. Objectives
- Drive compilation of user-submitted C++ source files (`g++ -O2 -std=c++17`) and capture compiler diagnostics for `Compilation Error` (CE) handling.
- Enforce CPU execution time limits (e.g. 1000ms) using steady-clock process watchdogs to detect `Time Limit Exceeded` (TLE).
- Enforce working set memory limits (e.g. 64MB) via OS memory sampling (`GetProcessMemoryInfo` on Windows) to detect `Memory Limit Exceeded` (MLE).
- Compare actual program outputs against reference test cases with trailing whitespace tolerance (`Accepted` / `Wrong Answer`).
- Honestly document security boundaries and sandboxing limitations.

## 4. Features
- **Standardized Competitive Programming Verdicts**: Emits `AC`, `WA`, `TLE`, `MLE`, `RE`, and `CE`.
- **Active Process Monitoring**: Real-time process termination upon constraint violation via `TerminateProcess` / `kill`.
- **Whitespace-Tolerant Diff Engine**: Compares line tokens while handling carriage return `\r\n` differences.
- **Honest Sandboxing Documentation**: Transparently highlights the boundaries of process-level monitoring versus kernel sandboxes.

## 5. Technology Stack
- **Language**: C++17 (`std::vector`, `std::string`, `std::chrono`, `std::filesystem`).
- **OS Process & Memory APIs**: Win32 `CreateProcessA`, `WaitForSingleObject`, `GetProcessMemoryInfo` (`psapi.lib`).
- **External Services**: None. Fully offline and self-contained.
- **Database**: None required.
- **Testing**: `doctest` unit testing framework.

## 6. Folder Structure
```
18-Coding-Test-Platform/
├── CMakeLists.txt
├── README.md
├── .gitignore
├── include/
│   ├── ExecutionSandbox.hpp
│   └── JudgeEngine.hpp
├── src/
│   ├── ExecutionSandbox.cpp
│   ├── JudgeEngine.cpp
│   └── main.cpp
└── tests/
    └── test_judge_platform.cpp
```

## 7. Installation / Prerequisites
- CMake 3.20 or newer.
- GCC 11+, Clang 14+, or MSVC 2022.
- doctest headers.

## 8. Build and Run Instructions
### Windows & Linux Build
```bash
cmake -S . -B build -G Ninja
cmake --build build
```

### Running the Application
```bash
./build/CodingTestPlatform
```

### Automated Verification Mode
```bash
./build/CodingTestPlatform --test-run
```

## 9. Testing Instructions
Run CTest:
```bash
ctest --test-dir build --output-on-failure
```

## 10. Console Output Sample
```
=================================================
    Automated Coding Test Platform & Judge       
=================================================

[!] SANDBOXING & RESOURCE LIMITATION NOTICE:
  1. Host Process Limits: Execution time and memory are enforced via OS
     watchdogs (WaitForSingleObject / GetProcessMemoryInfo) and kill signals.
  2. Security Boundary: Windows processes run under current user privileges.
     Enterprise competitive platforms must run in isolated unprivileged containers
     (Linux cgroups / seccomp / Docker / gVisor) to isolate syscall access.
-------------------------------------------------
Problem: 'Two-Sum Addition'
Evaluating 3 test cases (Time Limit: 1000ms, Memory Limit: 64MB)...

[Evaluating Correct Solution -> Expected AC]
Verdict: AC (Accepted)
  Test #1: AC (Accepted) (12 ms, 4096 KB)
  Test #2: AC (Accepted) (12 ms, 4096 KB)
  Test #3: AC (Accepted) (12 ms, 4096 KB)

[Evaluating Erroneous Solution -> Expected WA]
Verdict: WA (Wrong Answer)

[Evaluating Infinite Loop Solution -> Expected TLE]
Verdict: TLE (Time Limit Exceeded)

[Test Mode] Code evaluation platform verified.
```

## 11. Known Sandboxing Limitations
- **Lack of Kernel Namespaces on Windows**: In native Windows CLI environments, child processes execute within the caller's user session. Host file system writes or network socket calls are not virtualized without running inside Docker containers or virtual machines.
- **Recommendation for Untrusted Submissions**: Always execute untrusted student or user submissions inside ephemeral, rootless Linux containers equipped with `seccomp-bpf` syscall filters, read-only root filesystems, and disabled network access (`--net=none`).

## 12. Testing Status
- **Built**: Yes, with GCC 16.1.0 on Windows linking `psapi` with 0 warnings.
- **Run**: Verified AC, WA, TLE, and CE evaluations with `--test-run`.
- **Tested**: All unit test suites passed under CTest.
