# Project 03: AI-Chatbot-Tool

## 1. Project Overview
`AI-Chatbot-Tool` is an LLM artificial intelligence conversational client. It manages multi-turn dialogue histories, serializes OpenAI-compatible JSON payloads, connects via secure HTTPS REST APIs, and features a deterministic local mock engine for offline testing.

## 2. Problem Statement
Production AI integrations must cleanly manage asynchronous HTTP communication with LLM API endpoints, securely ingest credentials from environment variables without committing secrets, serialize chat context buffers, and provide deterministic offline fixtures for CI pipelines.

## 3. Objectives
- Ingest `LLM_API_KEY` and `LLM_ENDPOINT_URL` securely from system environment variables with `.env.example` guidance.
- Maintain rolling conversation message history with System, User, and Assistant roles.
- Serialize request bodies and parse JSON responses using `nlohmann/json`.
- Implement a deterministic local mock LLM provider ensuring 100% test pass rate in offline environments.

## 4. Features
- **Zero-Credential Leakage**: No hardcoded API keys; `.env.example` template provided.
- **Context Preservation**: Rolling multi-turn message history buffers context across turns.
- **Dual Mode (Live/Mock)**: Automatically detects when credentials or network are unavailable and seamlessly falls back to offline mock mode.
- **Token Analytics**: Tracks prompt and completion token counts from response metadata.

## 5. Technology Stack
- **Language**: C++17
- **Compiler**: GCC 11+, Clang 14+, MSVC 2022
- **Build System**: CMake 3.20+
- **JSON**: `nlohmann/json` 3.11+
- **Networking**: WinINet (Windows) / libcurl (Linux)
- **Testing**: doctest 2.4+ via CTest
- **External Services**: OpenAI / Gemini REST endpoints (optional; offline mock included)

## 6. Folder Structure
```
03-AI-Chatbot-Tool/
├── include/
│   └── ChatbotClient.hpp
├── src/
│   ├── ChatbotClient.cpp
│   └── main.cpp
├── tests/
│   └── test_chatbot.cpp
├── CMakeLists.txt
├── vcpkg.json
├── .env.example
├── README.md
└── .gitignore
```

## 7. Installation / Prerequisites
- CMake 3.20+
- C++17 compliant compiler
- Windows SDK (WinINet) or `libcurl4-openssl-dev` on Linux

## 8. Build and Run Instructions
### Windows & Linux
```bash
cmake -S . -B build -G Ninja
cmake --build build

# Interactive chat:
./build/AIChatbotTool

# Headless scenario verification:
./build/AIChatbotTool --test-run
```

## 9. Testing Instructions
```bash
ctest --test-dir build --output-on-failure
```

## 10. Console Output Sample
```
=================================================================
             LLM AI CHATBOT TOOL (API & MOCK ENGINE)             
=================================================================
[Test-Run Mode] Querying assistant with test prompt...

User : What is the core advantage of RAII in C++?
AI   : RAII (Resource Acquisition Is Initialization) ties resource lifespan to object lifetime. Destructors guarantee cleanup.
Stats: [MOCK MODE] | Prompt Tokens: 10 | Completion Tokens: 42

[Test-Run Mode] Completed successfully.
```

## 11. Known Limitations & Testing Boundaries
- Live API calls require a valid OpenAI/Gemini API key exported in `LLM_API_KEY`. Without an active key, the system automatically runs in deterministic offline mock mode.

## 12. Testing Status
- **Built**: Yes (Clean compilation under GCC 16.1.0 on Windows, 0 warnings).
- **Run**: Verified interactive launch and `--test-run` flag.
- **Tested**: 3 test suites covering history serialization, JSON response parsing, and mock mode passing under CTest.
