# Project 08: Image-Filter-Tool

## 1. Project Overview
`Image-Filter-Tool` is an image processing application that applies image enhancement, color transformation, and spatial convolution algorithms to raw raster images. It directly parses and encodes both Netpbm binary PPM (P6) and 24-bit uncompressed Windows Bitmap (BMP) formats.

## 2. Problem Statement
Digital image manipulation requires understanding raw pixel raster buffers, memory alignment and stride padding, color space luminance transformations, and $3 \times 3$ discrete convolution kernels for filtering.

## 3. Objectives
- Implement direct binary format loaders/savers for PPM and BMP formats with stride padding calculation.
- Deliver spatial convolution filters ($3 \times 3$ kernel matrices) with boundary clamping.
- Implement Gaussian blur, Laplacian sharpen, and Sobel gradient magnitude edge detection.
- Provide color transformations including NTSC luminance grayscale, color inversion, and linear brightness adjustments.

## 4. Features
- **Format Parsers**: Direct binary Netpbm P6 PPM and 24-bit RGB BMP (bottom-to-top with 4-byte row padding).
- **Spatial Convolution Engine**: Arbitrary $3 \times 3$ kernels with custom divisors and DC bias offsets.
- **Sobel Edge Detection**: Approximates 2D spatial gradient magnitude ($M = \sqrt{G_x^2 + G_y^2}$) to highlight high-contrast boundaries.
- **Fast In-Memory Testing**: Complete unit test coverage for pixel math, convolution, and file roundtrips.

## 5. Technology Stack
- **Language**: C++17
- **Compiler**: GCC 11+, Clang 14+, MSVC 2022
- **Build System**: CMake 3.20+
- **Testing**: doctest 2.4+ via CTest
- **Dependencies**: Standard library only (no external OpenCV or heavy runtime dependencies required)
- **External Services**: None (100% offline)

## 6. Folder Structure
```
08-Image-Filter-Tool/
├── include/
│   ├── Image.hpp
│   └── ImageFilter.hpp
├── src/
│   ├── Image.cpp
│   ├── ImageFilter.cpp
│   └── main.cpp
├── tests/
│   └── test_image_filter.cpp
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

# Headless verification and synthetic image generation:
./build/ImageFilterTool --test-run
```

## 9. Testing Instructions
```bash
ctest --test-dir build --output-on-failure
```

## 10. Console Output Sample
```
=================================================================
              IMAGE FILTER TOOL (CONVOLUTION & COLOR)             
=================================================================
[Test-Run Mode] Generating 64x64 synthetic image and applying filters...
Filter transformations succeeded:
  - Grayscale:       64x64 px
  - Invert:          64x64 px
  - Gaussian Blur:   64x64 px
  - Sharpen:         64x64 px
  - Sobel Edges:     64x64 px
Synthetic input and output exported to PPM and BMP cleanly.
```

## 11. Known Limitations
- Supported input formats are PPM (P6) and 24-bit uncompressed BMP. Compressed formats (JPEG/PNG) require stb_image or libpng.

## 12. Testing Status
- **Built**: Yes (Clean compilation under GCC 16.1.0 on Windows, 0 warnings).
- **Run**: Verified `--test-run` and synthetic image generation.
- **Tested**: 3 test suites covering raster operations, filter math, and BMP/PPM roundtrips passing under CTest.
