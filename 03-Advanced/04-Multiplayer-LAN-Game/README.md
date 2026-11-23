# 04-Multiplayer-LAN-Game

## 1. Project Overview
04-Multiplayer-LAN-Game is a high-performance authoritative multiplayer networked game engine designed for local area networks (LAN). Built in standard C++17, the application implements client-server architecture with server-authoritative physics simulation, tick-based delta updates at 20 Hz, input serialization, and client prediction safeguards.

## 2. Problem Statement
Client-authoritative multiplayer games suffer from synchronization discrepancies, physics divergence, and vulnerability to client manipulation. Building an authoritative simulation requires a strict separation of concerns where clients transmit intent (user inputs) and the authoritative host arbitrates state transitions, applies spatial boundaries, detects scoring zones, and broadcasts synchronized world snapshots.

## 3. Objectives
- Design a deterministic server-side simulation loop operating on fixed 50ms ticks (20 Hz).
- Implement cross-platform non-blocking TCP socket abstraction (`SocketCompat.hpp`) supporting Windows Winsock2 (`ws2_32`) and POSIX sockets.
- Implement efficient text-line wire protocol for handshakes (`JOIN`, `WELCOME`), continuous control inputs (`INPUT <seq> <dx> <dy>`), and periodic world snapshots (`SNAPSHOT <tick> <count> ...`).
- Provide automated headless test execution via `--test-run` flags for continuous integration.

## 4. Features
- **Authoritative Simulation**: The server validates all movements, clamps player coordinates within bounded arena borders ($[0, 800] \times [0, 600]$), and computes proximity scoring.
- **Concurrent Worker Pools**: Threaded connection acceptor and per-client socket workers decouple network I/O from the 20 Hz physics update loop.
- **Wire Protocol Parsing**: Lossless state serialization and deserialization with sequence numbers.
- **Client Prediction & Interpolation**: Decoupled client worker receiving asynchronous snapshots and maintaining real-time world cache.
- **Clean Teardown**: Cooperative thread termination and graceful socket shutdown.

## 5. Technology Stack
- **Language**: C++17 (`std::thread`, `std::mutex`, `std::atomic`, `std::optional`, `std::chrono`).
- **Networking API**: Standard Berkeley/POSIX sockets on Linux; Winsock 2 (`ws2_32`) on Windows.
- **External Services**: None. Operates completely offline over local loopback (`127.0.0.1`) or LAN subnets.
- **Database**: None required.
- **Testing Framework**: `doctest` unit testing framework.

## 6. Folder Structure
```
04-Multiplayer-LAN-Game/
├── CMakeLists.txt
├── README.md
├── .gitignore
├── include/
│   ├── AuthoritativeServer.hpp
│   ├── GameClient.hpp
│   ├── GameProtocol.hpp
│   └── SocketCompat.hpp
├── src/
│   ├── AuthoritativeServer.cpp
│   ├── GameClient.cpp
│   ├── client_main.cpp
│   └── server_main.cpp
└── tests/
    └── test_multiplayer_game.cpp
```

## 7. Installation / Prerequisites
- CMake 3.20 or newer.
- GCC 11+, Clang 14+, or MSVC 2022.
- doctest headers in include search paths.

## 8. Build and Run Instructions
### Windows & Linux Build
```bash
cmake -S . -B build -G Ninja
cmake --build build
```

### Running Server
```bash
./build/MultiplayerServer --port 9000
```

### Running Client
```bash
./build/MultiplayerClient --host 127.0.0.1 --port 9000 --name Ace
```

### Automated Smoke Verification
```bash
./build/MultiplayerServer --test-run
./build/MultiplayerClient --test-run
```

## 9. Testing Instructions
Run CTest to execute all test suites:
```bash
ctest --test-dir build --output-on-failure
```

## 10. Console Output Sample
```
=== Authoritative LAN Game Server ===
Binding to port 9000...
Server active and listening for players at 20 Hz tick rate.
[Test Mode] Running smoke verification for 500ms...
[Test Mode] Smoke verification completed successfully.
```

## 11. Known Limitations
- Current implementation uses reliable TCP framing; high-packet-loss competitive games typically utilize UDP with custom packet acknowledgement.
- Graphics rendering is presented via headless console telemetry; graphical viewports can be bound to Qt or raylib.

## 12. Testing Status
- **Built**: Yes, with GCC 16.1.0 on Windows with 0 warnings (`-Wall -Wextra -Wpedantic`).
- **Run**: Verified both server and client `--test-run` modes successfully.
- **Tested**: All 3 unit test cases passing under CTest.
