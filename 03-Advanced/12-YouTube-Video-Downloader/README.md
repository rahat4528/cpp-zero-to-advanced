# 12-YouTube-Video-Downloader

## 1. Project Overview
> **LEGAL AND TERMS OF SERVICE NOTICE**:
> This software is designed strictly for educational, archival, and research purposes. Users must strictly utilize this software only for content that they legally own, author, or have explicit permission/license to download. Bypassing technological protection measures or downloading copyrighted content without authorization violates YouTube's Terms of Service and international copyright statutes (such as the Digital Millennium Copyright Act / DMCA). The authors and contributors assume no liability for misuse.

12-YouTube-Video-Downloader is a chunked media stream download utility developed in standard C++17. The project implements HTTP range-based multi-part stream acquisition, dynamic download speed calculations, progress callback hooks, and an offline mock media streaming server (`MockMediaServer`) for deterministic verification.

## 2. Problem Statement
Media streaming services deliver video content in segmented multi-megabyte streams or multi-rate HTTP live streams (HLS/DASH). Downloading large media streams requires chunk-based recovery, network buffer management, real-time throughput estimation, and non-blocking progress updates.

## 3. Objectives
- Implement a chunked binary streaming engine with configurable buffer sizes.
- Provide real-time progress callbacks calculating percent completion, transfer rates (KB/s), and elapsed time.
- Implement a mock media streaming backend (`MockMediaServer`) to provide headless CI testing without external internet dependencies.
- Embed explicit legal disclaimers regarding platform Terms of Service and intellectual property law.
- Provide automated `--test-run` execution and unit test verification under CTest.

## 4. Features
- **Chunked Transfer Architecture**: Consumes media streams in discrete chunks to bound memory footprint.
- **Dynamic Speed Calculation**: Instantaneous throughput tracking updated across time deltas.
- **Cross-Platform Native Networking**: Utilizes Windows WinINet API on Windows and standard HTTP streaming on POSIX platforms.
- **Local Simulation Engine**: Generates verifiable binary media streams for tests without external network latency.

## 5. Technology Stack
- **Language**: C++17 (`std::vector`, `std::string`, `std::chrono`, `std::function`, `std::filesystem`).
- **Networking API**: WinINet (`wininet.lib`) on Windows; POSIX sockets/HTTP.
- **External Services**: None required in default/test mode.
- **Database**: None required.
- **Testing**: `doctest` unit testing framework.

## 6. Folder Structure
```
12-YouTube-Video-Downloader/
├── CMakeLists.txt
├── README.md
├── .gitignore
├── include/
│   ├── ChunkDownloader.hpp
│   └── MockMediaServer.hpp
├── src/
│   ├── ChunkDownloader.cpp
│   ├── MockMediaServer.cpp
│   └── main.cpp
└── tests/
    └── test_video_downloader.cpp
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
./build/VideoDownloader --id sampleVideoId
```

### Automated Verification Mode
```bash
./build/VideoDownloader --test-run
```

## 9. Testing Instructions
Run CTest:
```bash
ctest --test-dir build --output-on-failure
```

## 10. Console Output Sample
```
===============================================
       Media Stream Chunk Downloader           
===============================================
LEGAL NOTICE: This utility is strictly intended for downloading
media content you own or have explicit rights/licenses to obtain.
Downloading copyrighted materials without authorization violates
platform Terms of Service and applicable intellectual property laws.
-----------------------------------------------
Target Title:  Educational C++ Modern Architecture Lecture (dQw4w9WgXcQ)
MIME Type:     video/mp4
Content Size:  512 KB

Initiating chunk stream acquisition...
[Download Progress] 100.0% (512 / 512 KB) Speed: 3200.0 KB/s
Download completed successfully to: sample_downloaded_video.bin
[Test Mode] Chunk downloader and progress monitoring verified.
```

## 11. Known Limitations
- Does not decrypt DRM-protected streams or execute client-side script engine signature deciphers; operates on direct media chunk URLs.

## 12. Testing Status
- **Built**: Yes, with GCC 16.1.0 on Windows linking `wininet` with 0 warnings.
- **Run**: Verified chunk streaming and progress monitoring with `--test-run`.
- **Tested**: All unit test suites passed under CTest.
