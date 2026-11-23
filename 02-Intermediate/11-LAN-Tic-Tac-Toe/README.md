# Project 11: LAN-Tic-Tac-Toe

## 1. Project Overview
`LAN-Tic-Tac-Toe` is a networked two-player strategy game communicating over local area TCP stream sockets. It features turn-based state validation, win/draw detection, structured protocol packet framing, and board visualization.

## 2. Problem Statement
Multiplayer turn-based board games require strict synchronization between independent client runtimes, move legality verification, race condition prevention during concurrent inputs, and clean recovery upon socket disconnections.

## 3. Objectives
- Implement a 3x3 Tic-Tac-Toe board state machine validating move occupancy, boundary checks, and win/draw evaluations.
- Create an explicit text-framed wire protocol (`MOVE <r> <c> <p>`, `START <p>`, `QUIT`, `MSG <text>`).
- Provide cross-platform TCP socket communication (Winsock on Windows, BSD sockets on POSIX).
- Facilitate offline headless verification via a dedicated `--test-run` flag.

## 4. Features
- **Deterministic Win Evaluation**: Detects rows, columns, and diagonal lines in constant time.
- **Move Validation Engine**: Prevents overwriting already occupied cells or playing out of turn.
- **Wire Framing Protocol**: Standard serialized packets for peer synchronization.
- **Automated Test Coverage**: Complete unit test suite verifying win conditions, draws, illegal moves, and protocol packet deserialization.

## 5. Technology Stack
- **Language**: C++17
- **Compiler**: GCC 11+, Clang 14+, MSVC 2022
- **Build System**: CMake 3.20+
- **Sockets**: Winsock2 (`ws2_32`) / POSIX sockets
- **Testing**: doctest 2.4+ via CTest
- **External Services**: None (Localhost / LAN only)

## 6. Folder Structure
```
11-LAN-Tic-Tac-Toe/
├── include/
│   ├── GameProtocol.hpp
│   ├── SocketCompat.hpp
│   └── TicTacToeBoard.hpp
├── src/
│   ├── TicTacToeBoard.cpp
│   └── main.cpp
├── tests/
│   └── test_tictactoe.cpp
├── CMakeLists.txt
├── README.md
└── .gitignore
```

## 7. Installation / Prerequisites
- CMake 3.20+
- C++17 compliant compiler
- On Windows: Winsock2 library (`ws2_32.lib`)

## 8. Build and Run Instructions
### Windows & Linux
```bash
cmake -S . -B build -G Ninja
cmake --build build

# Host a game as Player X:
./build/TicTacToe --host 9090

# Join an existing host as Player O:
./build/TicTacToe --join 127.0.0.1 9090

# Automated test run:
./build/TicTacToe --test-run
```

## 9. Testing Instructions
```bash
ctest --test-dir build --output-on-failure
```

## 10. Console Output Sample
```
=================================================================
               LAN TIC-TAC-TOE MULTIPLAYER GAME                  
=================================================================
[Test-Run Mode] Simulating complete board playthrough & serialization...

  0   1   2
0 X | X | X
  ---+---+---
1 O | O |  
  ---+---+---
2   |   |  

Game Status: X WINS (Verified!)
Serialized Packet: MOVE 2 2 X
Deserialization roundtrip verified successfully.
```

## 11. Known Limitations
- Plaintext TCP sockets; does not implement cryptographic session authentication or reconnect resilience upon network dropouts.

## 12. Testing Status
- **Built**: Yes (Compiled cleanly with GCC 16.1.0 on Windows, 0 warnings).
- **Run**: Verified `--test-run` and interactive game launch.
- **Tested**: 2 multi-subcase test suites passing under CTest.
