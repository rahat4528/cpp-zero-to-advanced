# Project 01: Face-Detection-App

## 1. Project Overview
`Face-Detection-App` is a computer vision facial candidate identification engine implementing an integral image Viola-Jones Haar-like cascade classifier pipeline with sliding window pyramid scaling and Non-Maximum Suppression (NMS).

## 2. Problem Statement
Automated facial detection across unconstrained visual inputs requires fast, scale-invariant feature extraction capable of computing multi-rectangle luminance variations in $O(1)$ constant time per candidate window.

## 3. Objectives
- Compute summed-area tables (Integral Images) allowing arbitrary sub-rectangle pixel sums in $O(1)$ time.
- Implement Haar-like feature primitives (two-rectangle horizontal/vertical, three-rectangle horizontal).
- Multi-scale sliding window pyramid with early cascade stage rejection.
- Non-Maximum Suppression (NMS) bounding box clustering via Intersection over Union (IoU).
- Provide drop-in binding for OpenCV (`find_package(OpenCV QUIET)`) while maintaining a self-contained, dependency-free internal vision pipeline.

## 4. Features
- **$O(1)$ Integral Image Acceleration**: Rapid rectangular luminance sum evaluation.
- **Cascade Stage Rejection**: Prunes negative non-face candidates within early stages to maximize throughput.
- **Bounding Box Clustering (NMS)**: Eliminates duplicate detections with configurable IoU thresholds.
- **Headless Verification**: `--test-run` flag testing synthetic visual matrices without requiring webcam hardware.

## 5. Technology Stack
- **Language**: C++17
- **Compiler**: GCC 11+, Clang 14+, MSVC 2022
- **Build System**: CMake 3.20+
- **Computer Vision**: Custom high-performance Haar-cascade engine + optional OpenCV integration
- **Testing**: doctest 2.4+ via CTest

## 6. Folder Structure
```
01-Face-Detection-App/
├── include/
│   └── FaceDetector.hpp
├── src/
│   ├── FaceDetector.cpp
│   └── main.cpp
├── tests/
│   └── test_face_detector.cpp
├── CMakeLists.txt
├── vcpkg.json
├── README.md
└── .gitignore
```

## 7. Installation / Prerequisites
- CMake 3.20+
- C++17 compliant compiler
- Optional: OpenCV 4+ (`vcpkg install opencv4`)

## 8. Build and Run Instructions
### Windows & Linux
```bash
cmake -S . -B build -G Ninja
cmake --build build

# Headless verification mode:
./build/FaceDetectionApp --test-run
```

## 9. Testing Instructions
```bash
ctest --test-dir build --output-on-failure
```

## 10. Console Output Sample
```
=================================================================
          COMPUTER VISION FACE DETECTION APP (HAAR CASCADE)      
=================================================================
[Test-Run Mode] Generating synthetic raster with facial features...
Found 1 facial candidate region(s):
  Region [1]: (20, 20) 48x48 px | Confidence: 45.20
[Test-Run Mode] Detection pipeline executed successfully.
```

## 11. Known Limitations & Testing Boundaries
- Physical camera capture and deep neural network models (DNN ResNet-10 SSD) require OpenCV video I/O backends; the self-contained pipeline verifies detection on 8-bit luminance rasters.

## 12. Testing Status
- **Built**: Yes (Clean compilation under GCC 16.1.0 on Windows, 0 warnings).
- **Run**: Verified `--test-run` and synthetic feature detection.
- **Tested**: 3 test suites covering Integral Image math, IoU calculation, and NMS suppression passing under CTest.
