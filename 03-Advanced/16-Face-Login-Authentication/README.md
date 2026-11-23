# 16-Face-Login-Authentication

## 1. Project Overview
16-Face-Login-Authentication is a biometric facial recognition authentication system developed in modern C++17. The engine implements spatial bilinear image resampling, Local Binary Patterns (LBP) micro-texture descriptor extraction across partitioned facial grid cells, Chi-Square ($\chi^2$) histogram distance matching, and an enrollment template vault.

## 2. Problem Statement
Password authentication is vulnerable to credential theft, phishing, and dictionary brute-force attacks. Biometric facial authentication replaces or augments knowledge-based secrets with physical characteristics. However, deploying face recognition requires rigorous understanding of false accept rates (FAR), false reject rates (FRR), and known anti-spoofing limitations.

## 3. Objectives
- Implement bilinear interpolation to standardize incoming facial probes to a normalized $32 \times 32$ ROI.
- Extract Local Binary Patterns (LBP) across $4 \times 4$ spatial cells with 8-neighbor circular pixel comparisons, yielding a 256-dimensional texture histogram descriptor.
- Compute Chi-Square distance metrics between candidate probes and stored enrollment templates.
- Enforce strict distance thresholding ($\text{threshold} \le 0.35$) to grant or deny system access.
- Clearly document biometric security limits (2D spoofing, liveness absence, lighting variance).

## 4. Features
- **LBP Texture Invariance**: Local Binary Patterns offer robustness against monotonic grayscale shifts and illumination fluctuations.
- **Spatially Enhanced Histograms**: Dividing the facial region into grid cells preserves topological feature arrangements (eyes, nose, mouth).
- **Non-Parametric Distance Matching**: Chi-Square metric weights sparse feature bins effectively.
- **Explicit Security Disclaimers**: Educates developers on 2D presentation attacks and liveness requirements.

## 5. Technology Stack
- **Language**: C++17 (`std::vector`, `std::string`, `std::map`, `std::cmath`, `std::algorithm`).
- **External Services**: None. Self-contained local biometric matcher.
- **Database**: None required.
- **Testing**: `doctest` unit testing framework.

## 6. Folder Structure
```
16-Face-Login-Authentication/
├── CMakeLists.txt
├── README.md
├── .gitignore
├── include/
│   ├── FaceAuthManager.hpp
│   └── FaceDescriptor.hpp
├── src/
│   ├── FaceAuthManager.cpp
│   ├── FaceDescriptor.cpp
│   └── main.cpp
└── tests/
    └── test_face_auth.cpp
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
./build/FaceLoginAuth
```

### Automated Verification Mode
```bash
./build/FaceLoginAuth --test-run
```

## 9. Testing Instructions
Run CTest:
```bash
ctest --test-dir build --output-on-failure
```

## 10. Console Output Sample
```
================================================
   Biometric Face Login Authentication Engine   
 (Local Binary Patterns & Chi-Square Matching)  
================================================

[!] SECURITY LIMITATION NOTICE:
  1. 2D Presentation Attack Vulnerability: Standard 2D facial descriptors
     cannot distinguish a live human face from a photograph or digital display.
  2. Environmental Drift: Extreme lighting variations, shadows, and occlusions
     (masks/glasses) alter texture gradients and affect false rejection rates (FRR).
  3. Production Recommendation: Multi-factor biometric fusion with active liveness
     challenge-response (e.g. eye blink / head yaw detection) or structured infrared depth.
------------------------------------------------
Enrolled 2 users in biometric template vault.

Attempting Authentication for Alice (Genuine Probe)...
Result: [SUCCESS] -> Access Granted to 'Alice' (Chi2 distance: 0.0054 <= threshold 0.3500)

Attempting Authentication for Imposter (Eve)...
Result: [REJECTED] -> Access Denied: Imposter or unrecognized face (Closest match: 'Bob', Chi2 distance: 0.6842 > threshold 0.3500)

[Test Mode] Biometric LBP face authentication verified.
```

## 11. Known Limitations & Security Vulnerabilities
- **2D Presentation Attacks**: This system operates on monocular 2D intensity images and does not incorporate passive or active liveness detection (infrared structured light, time-of-flight depth, micro-expression tracking). As a result, static printed photographs or tablet screens could spoof recognition.
- **Pose & Expression Sensitivity**: Yaw/pitch angles exceeding 15 degrees reduce recognition accuracy.

## 12. Testing Status
- **Built**: Yes, with GCC 16.1.0 on Windows with 0 warnings (`-Wall -Wextra -Wpedantic`).
- **Run**: Verified LBP feature extraction, genuine login approval, and imposter rejection with `--test-run`.
- **Tested**: All unit test suites passed under CTest.
