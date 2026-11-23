# 15-OCR-Scanner-Tool

## 1. Project Overview
15-OCR-Scanner-Tool is an Optical Character Recognition (OCR) computer vision pipeline developed in standard C++17. Designed to provide self-contained optical scanning without heavyweight external binary bloat, it implements Otsu's adaptive bimodal image thresholding, connected component flood-fill bounding-box segmentation, $8 \times 8$ raster grid normalization, structural template Hamming distance classification, and topological word-spacing reconstruction.

## 2. Problem Statement
Extracting readable text from digitized scanned documents or screen buffers requires computer vision preprocessing: isolating ink from varying lighting backgrounds, discovering discrete connected character glyphs, normalizing uneven bounding aspect ratios, and classifying glyph representations against reference fonts.

## 3. Objectives
- Implement Otsu's automatic variance-maximizing thresholding to binarize grayscale images into foreground and background.
- Implement connected component analysis (CCA) via breadth-first search (BFS) flood filling to delineate individual character bounding boxes.
- Extract and resample bounding boxes into standardized $8 \times 8$ normalized binary matrices.
- Classify glyphs using minimum Hamming distance against registered font templates (`0-9`, `A-Z`).
- Assemble character sequences into words and sentences based on inter-glyph coordinate gaps.
- Provide automated CI test execution via `--test-run` and doctest unit tests.

## 4. Features
- **Adaptive Image Binarization**: Otsu's algorithm calculates the bimodal variance cutoff dynamically for each image.
- **Topological Glyph Segmentation**: Robust BFS component discovery filters single-pixel noise and groups ink pixels into glyph contours.
- **Multi-Glyph Lexical Reconstruction**: Automates reading-order sorting (top-to-bottom, left-to-right) and word space insertion.
- **Zero-Dependency Architecture**: Entire computer vision and classification pipeline compiles natively with standard C++17 compilers without requiring external Leptonica or Tesseract dynamic libraries.

## 5. Technology Stack
- **Language**: C++17 (`std::vector`, `std::string`, `std::queue`, `std::cmath`, `std::algorithm`).
- **External Services**: None. Fully offline and self-contained.
- **Database**: None required.
- **Testing**: `doctest` unit testing framework.

## 6. Folder Structure
```
15-OCR-Scanner-Tool/
├── CMakeLists.txt
├── README.md
├── .gitignore
├── include/
│   ├── ImageRaster.hpp
│   └── OCREngine.hpp
├── src/
│   ├── ImageRaster.cpp
│   ├── OCREngine.cpp
│   └── main.cpp
└── tests/
    └── test_ocr_scanner.cpp
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
./build/OCRScanner
```

### Automated Verification Mode
```bash
./build/OCRScanner --test-run
```

## 9. Testing Instructions
Run CTest:
```bash
ctest --test-dir build --output-on-failure
```

## 10. Console Output Sample
```
========================================
    Optical Character Recognition Tool  
  (Otsu Thresholding & Glyph Classifier)
========================================
Created synthetic raster bitmap (120x30 px)...
Applying Otsu bimodal binarization and connected component segmentation...

----------------------------------------
Target Text:     "CPP 2026"
Recognized Text: "CPP 2026"
----------------------------------------
[Test Mode] OCR segmentation and character classification verified.
```

## 11. Known Limitations
- The built-in template matcher is optimized for standard monospaced/sans-serif glyph forms ($8 \times 8$ normalized resolution); proportional handwriting recognition typically incorporates neural LSTM classifiers.

## 12. Testing Status
- **Built**: Yes, with GCC 16.1.0 on Windows with 0 warnings (`-Wall -Wextra -Wpedantic`).
- **Run**: Verified Otsu thresholding, connected component segmentation, and synthetic character recognition with `--test-run`.
- **Tested**: All 3 test cases passed under CTest.
