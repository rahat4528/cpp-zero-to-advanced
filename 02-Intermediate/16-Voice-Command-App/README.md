# Project 16: Voice-Command-App

## 1. Project Overview
`Voice-Command-App` is an offline acoustic voice command processing and intent dispatch engine. It provides grammar-based speech recognition matching, fuzzy Levenshtein phonetic distance tolerance, and action execution callbacks.

## 2. Problem Statement
Speech-driven user interfaces operating in embedded, air-gapped, or privacy-sensitive contexts require local, offline command decoding without routing audio buffers to third-party cloud transcription providers.

## 3. Objectives
- Establish a decoupled speech recognition grammar engine supporting phonetic and lexical token matching.
- Provide a fuzzy acoustic matching algorithm with Levenshtein distance tolerance to handle background noise, slurred speech, or dialectal variances.
- Implement an intent dispatch registry triggering strongly typed callbacks for recognized system commands.
- Transparently document hardware specifications, audio sampling constraints, and offline acoustic model boundaries.

## 4. Features
- **Offline Grammar Engine**: Recognizes commands (`system status`, `volume up/down`, `mute`, `play/stop music`, `open application`) without network communication.
- **Acoustic Tolerance**: Tolerates phonetic slurs and minor acoustic dropped letters with parameterized confidence thresholds.
- **Action Callback Dispatcher**: Type-safe handler binding mapping voice intents to native functions.
- **Hardware Integration Architecture**: Formatted to accept 16 kHz 16-bit PCM mono input feeds from native capture APIs or lightweight Vosk C API recognizers.

## 5. Technology Stack
- **Language**: C++17
- **Compiler**: GCC 11+, Clang 14+, MSVC 2022
- **Build System**: CMake 3.20+
- **Testing**: doctest 2.4+ via CTest
- **External Services**: None (100% offline)

## 6. Folder Structure
```
16-Voice-Command-App/
├── include/
│   └── VoiceCommandEngine.hpp
├── src/
│   ├── VoiceCommandEngine.cpp
│   └── main.cpp
├── tests/
│   └── test_voice_command.cpp
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
./build/VoiceCommandApp --test-run
```

## 9. Testing Instructions
```bash
ctest --test-dir build --output-on-failure
```

## 10. Console Output Sample
```
=================================================================
             OFFLINE VOICE COMMAND DISPATCH ENGINE               
=================================================================
[Test-Run Mode] Feeding simulated acoustic voice utterances...

Heard utterance: "volume up"
  Matched Intent: Increase master audio gain by 10%
  -> [ACTION EXECUTED] Audio level boosted +10% (Confidence: 100.0%)

Heard utterance: "volum up"
  Matched Intent: Increase master audio gain by 10%
  -> [ACTION EXECUTED] Audio level boosted +10% (Confidence: 87.5%)

Heard utterance: "system status"
  Matched Intent: Display OS telemetry and resource metrics
  -> [ACTION EXECUTED] System CPU: 12% | RAM: 4.2 GB / 16 GB | Status: Optimal (Conf: 100.0%)
```

## 11. Known Limitations & Documented Hardware Constraints
- **Hardware Audio Sample Format**: Designed for 16,000 Hz sample rate, 16-bit signed integer Little Endian, single-channel (mono) PCM.
- **Acoustic Model Memory Footprint**: Production Vosk/Kaldi small English models require ~50MB to 120MB of resident RAM.
- **Latency Boundaries**: Offline neural/HMM decoding on low-power microcontrollers (e.g. ARM Cortex-M) introduces 200-500ms inference latency compared to near-zero latency on desktop x86_64 cores.

## 12. Testing Status
- **Built**: Yes (Clean compilation under GCC 16.1.0 on Windows, 0 warnings).
- **Run**: Verified `--test-run` and simulated speech dispatching.
- **Tested**: 2 multi-subcase test suites passing under CTest.
