# 05-Offline-Chat-App

A local conversational chat simulation engine featuring chatbot heuristics, two-user alternating mode, and transcript logging built with modern C++17.

---

## 1. Project Overview
Offline Chat App simulates a conversation environment operating entirely offline without network sockets. It provides automated chatbot heuristics, turn-based local multi-user messaging, and transcript file logging.

## 2. Problem Statement
Before building multi-threaded networked chat servers, engineers need to understand conversation state management, message serialization, conversational routing heuristics, slash command dispatching, and transcript persistence.

## 3. Objectives
- Model chat messages with senders, timestamps, and body strings.
- Implement rule-based intent parsing and response heuristics.
- Support both solo conversational bot mode and two-user turn-based local dialogue.
- Provide slash-command dispatching (`/help`, `/mode`, `/clear`, `/history`, `/export`).
- Export complete conversation transcripts to local disk files.

## 4. Features
- **Conversational Bot**: Rule-based bot understanding greetings, C++ queries, time queries, and defaults.
- **Two-User Local Mode**: Alternating turn-based chat between two local participants.
- **Slash Commands**: Runtime configuration and session manipulation.
- **Transcript Logging**: Timestamped logging with disk export.
- **Scripted Execution**: `--scripted "msg1;msg2"` allows automated testing without terminal pauses.

## 5. Technology Stack
- **Language**: C++17 (`std::vector`, `std::chrono`, `std::fstream`, `std::stringstream`, `std::string`)
- **Build System**: CMake 3.20+
- **Testing Framework**: doctest with CTest
- **Database Needed**: No. Plaintext transcript files.
- **Internet / External Service Needed**: No. Fully offline local simulation.

## 6. Folder Structure
```
05-Offline-Chat-App/
├── include/
│   └── OfflineChat.h         # Message structs and ChatEngine API
├── src/
│   ├── OfflineChat.cpp       # Bot heuristics, command parsing, log export
│   └── main.cpp              # Interactive terminal chat loop & CLI driver
├── tests/
│   └── test_offline_chat.cpp # doctest unit test suite
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

# Automated scripted conversation
.\build\OfflineChat.exe --scripted "hello;tell me about c++;time;/export demo_log.txt"

# Interactive two-user chat
.\build\OfflineChat.exe --mode two
```

### Linux / macOS
```bash
cmake -S . -B build -G Ninja
cmake --build build

./build/OfflineChat --scripted "hello;time;bye"
```

## 9. Testing Instructions
Run tests with CTest:
```bash
ctest --test-dir build --output-on-failure
```

## 10. Demo / Console Output Sample
```
$ ./build/OfflineChat --scripted "hello;what is modern c++?;time"
User: hello
CyberBot: Hello there! How can I assist you with modern C++ today?
User: what is modern c++?
CyberBot: Modern C++ (C++17/20) provides RAII, type safety, and zero-cost abstractions.
User: time
CyberBot: The local simulation time is 03:20:15.
```

## 11. Known Limitations
- Rule-based keyword matching without natural language embeddings (see `03-Advanced/03-AI-Chatbot-Tool` for LLM integration).
- Operates in-process locally without network sockets (see `02-Intermediate/04-Group-Chat-App` for TCP networking).

## 12. Testing Status
- **Built**: Yes, compiled with GCC 16.1.0 using `-Wall -Wextra -Wpedantic`.
- **Run**: Yes, tested both `--scripted` automated runs and file log exports.
- **Tested**: Yes, 5 unit test suites passing via CTest.
- **Hardware/Credentials Limitations**: None.
