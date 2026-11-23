# Project 19: Music-Player-App

## 1. Project Overview
`Music-Player-App` is a digital audio playback and playlist state machine. It manages track queues, looping and shuffle algorithms, gain volume clamping, and native parsing of RIFF/WAVE uncompressed audio stream metadata headers.

## 2. Problem Statement
Media applications require structured finite state machines (Playing, Paused, Stopped), deterministic playlist queue navigation (Repeat-All, Repeat-One, Shuffle), and robust binary container parsers to inspect audio stream properties without loading entire uncompressed streams into memory.

## 3. Objectives
- Implement an audio player finite-state machine with standard media controls (`play`, `pause`, `stop`, `next`, `previous`).
- Build playlist playback modes: Sequential (`Normal`), `RepeatOne`, `RepeatAll`, and pseudorandom `Shuffle`.
- Direct binary parser for RIFF/WAVE headers to extract sampling frequencies, channel layouts, bit depths, and playback duration.
- Isolate the state machine so it can drive miniaudio, Qt Multimedia, or Win32 WaveOut backends.

## 4. Features
- **State Machine**: Transitions safely across `Playing`, `Paused`, and `Stopped` states.
- **Repeat & Shuffle Modes**: Cyclic playlist wrap-around or uniform pseudorandom track selection.
- **WAV Header Parser**: Direct extraction of PCM format chunks (`RIFF`, `WAVE`, `fmt `, `data`).
- **Volume Regulation**: Gain normalization with automatic interval clamping $[0.0, 1.0]$.

## 5. Technology Stack
- **Language**: C++17
- **Compiler**: GCC 11+, Clang 14+, MSVC 2022
- **Build System**: CMake 3.20+
- **Testing**: doctest 2.4+ via CTest
- **Audio Architecture**: Self-contained state engine with RIFF parser and clean abstraction for miniaudio/Qt Multimedia bindings.
- **External Services**: None (100% offline)

## 6. Folder Structure
```
19-Music-Player-App/
├── include/
│   └── MusicPlayer.hpp
├── src/
│   ├── MusicPlayer.cpp
│   └── main.cpp
├── tests/
│   └── test_music_player.cpp
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

# Headless scenario verification:
./build/MusicPlayerApp --test-run
```

## 9. Testing Instructions
```bash
ctest --test-dir build --output-on-failure
```

## 10. Console Output Sample
```
=================================================================
               DIGITAL MUSIC PLAYER & PLAYLIST APP               
=================================================================
[Test-Run Mode] Populating playlist and testing state transitions...
Playlist size: 3 tracks
Now Playing: Clair de Lune by Claude Debussy (5:05)
Skipped to: Symphony No. 5 by Ludwig van Beethoven
Playback State: PAUSED
Volume Level: 95%
[Test-Run Mode] Completed successfully.
```

## 11. Known Limitations
- Hardware sound card rendering requires binding to a backend device like miniaudio or Qt Multimedia (the project currently provides the full audio container parser, playlist engine, and audio state controller).

## 12. Testing Status
- **Built**: Yes (Clean compilation under GCC 16.1.0 on Windows, 0 warnings).
- **Run**: Verified `--test-run` and state machine transitions.
- **Tested**: 2 multi-subcase test suites passing under CTest.
