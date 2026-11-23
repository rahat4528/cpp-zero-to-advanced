# Project 04: Group-Chat-App

## 1. Project Overview
`Group-Chat-App` is a multi-threaded client-server networked chat application built using TCP stream sockets and standard C++ threads. It delivers real-time group broadcast communication, direct private messaging (whispers), membership presence notifications, and active user discovery.

## 2. Problem Statement
Concurrent network communications require robust connection synchronization, non-blocking asynchronous message serialization, thread-safe client lifecycle management, and race-free message distribution across disparate client threads.

## 3. Objectives
- Architect a multi-client TCP server capable of handling simultaneous client sessions using standard C++ concurrency primitives (`std::thread`, `std::mutex`, `std::atomic`).
- Design an explicit framed line-delimited message protocol supporting public broadcast, private whispers, and roster queries.
- Build a responsive dual-threaded terminal client capable of concurrently receiving network packets while reading user console input.
- Provide clean cross-platform network socket abstraction (Winsock2 on Windows, POSIX sockets on Linux).

## 4. Features
- **Concurrent Server**: Accepts incoming connections in dedicated background threads without blocking ongoing conversations.
- **Framed Protocol Engine**: Message framing with pipe-delimited packet structure (`JOIN`, `MSG`, `WHISPER`, `USERS`, `LEAVE`, `SYSTEM`).
- **Private Whispers**: Direct user-to-user messaging with `/w <username> <message>`.
- **Roster Discovery**: Query currently connected active participants with `/users`.
- **Graceful Lifecycle**: Automated presence broadcasts when members enter or disconnect from the room.
- **Headless Test Support**: `--test-run` flag on server to support automated integration verification.

## 5. Technology Stack
- **Language**: C++17
- **Compiler**: GCC 11+, Clang 14+, MSVC 2022
- **Build System**: CMake 3.20+
- **Testing**: doctest 2.4+ via CTest
- **Sockets**: Winsock2 (`ws2_32` on Windows) / BSD sockets (Linux) via `SocketCompat.hpp`
- **Concurrency**: `std::thread`, `std::mutex`, `std::atomic`
- **External Services**: None (Localhost / LAN network only)
- **Database**: In-memory session registry

## 6. Folder Structure
```
04-Group-Chat-App/
├── include/
│   ├── ChatClient.hpp
│   ├── ChatProtocol.hpp
│   ├── ChatServer.hpp
│   └── SocketCompat.hpp
├── src/
│   ├── ChatClient.cpp
│   ├── ChatServer.cpp
│   ├── client_main.cpp
│   └── server_main.cpp
├── tests/
│   └── test_chat_protocol.cpp
├── CMakeLists.txt
├── README.md
└── .gitignore
```

## 7. Installation / Prerequisites
- CMake 3.20+
- C++17 compliant compiler
- On Windows: Windows SDK / Winsock2 (included with standard MinGW/MSVC toolchains)

## 8. Build and Run Instructions
### Windows & Linux
```bash
cmake -S . -B build -G Ninja
cmake --build build

# Start Server on port 8080:
./build/GroupChatServer 8080

# In another terminal, connect Client A:
./build/GroupChatClient 127.0.0.1 8080 Alice

# In another terminal, connect Client B:
./build/GroupChatClient 127.0.0.1 8080 Bob
```

## 9. Testing Instructions
```bash
ctest --test-dir build --output-on-failure
```

## 10. Console Output Sample
### Server
```
=================================================================
              MULTI-THREADED TCP GROUP CHAT SERVER               
=================================================================
[Server] Group Chat Server listening on port 8080...
```
### Client (Alice)
```
=================================================================
                   TCP GROUP CHAT CLIENT                         
=================================================================
Connected as 'Alice'. Start chatting!
> [!] Bob has entered the chat room.
> Hello everyone!
[@Bob]: Hey Alice, welcome!
[Whisper from @Bob]: Let's start the team meeting.
```

## 11. Known Limitations
- Network transmissions are unencrypted plaintext TCP streams; production systems would require TLS wrappers (OpenSSL / mbedTLS).

## 12. Testing Status
- **Built**: Yes (Compiled cleanly with GCC 16.1.0 on Windows, 0 warnings).
- **Run**: Verified both client and server launch and communication protocol works.
- **Tested**: 5/5 automated doctest unit test cases passing under CTest.
