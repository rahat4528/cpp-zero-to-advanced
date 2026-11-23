# Project 14: PDF-Generator-App

## 1. Project Overview
`PDF-Generator-App` is a PDF 1.4 document synthesizer and rendering engine written in standard C++. It constructs valid Adobe PDF documents supporting text typography, geometric vector primitives (lines, stroked/filled rectangles), coordinate translations, cross-reference (`xref`) tables, and font resources.

## 2. Problem Statement
Generating printable documents (such as invoices, certificates, and reports) typically introduces heavy external dependencies like libharu or Cairo. A lightweight, self-contained C++ PDF generation library avoids dynamic library DLL deployment hurdles while strictly conforming to the ISO PDF format.

## 3. Objectives
- Implement an object model for PDF pages, fonts, content streams, and document catalogs.
- Generate standard PDF 1.4 objects: Catalog dictionary, Pages tree, Page descriptors, Type 1 Font metrics, and content streams.
- Calculate exact byte offsets for the PDF cross-reference (`xref`) table and trailer dictionary.
- Render text runs and vector graphic shapes (rectangles, lines, custom line widths and RGB colors).

## 4. Features
- **Adobe PDF 1.4 Compliant**: Generates valid binary PDF files that open in Adobe Acrobat, Chrome, Preview, and standard PDF readers.
- **Vector Graphics Support**: Coordinate system rendering supporting stroked lines, rectangles, fill colors, and line widths.
- **Typography Engine**: Font binding supporting standard Type 1 fonts (Helvetica) with coordinate text positioning (`BT ... ET`, `Tf`, `Td`, `Tj`).
- **Complete Test Suite**: Unit tests validating PDF header markers, catalog objects, xref calculation, and stream encoding.

## 5. Technology Stack
- **Language**: C++17
- **Compiler**: GCC 11+, Clang 14+, MSVC 2022
- **Build System**: CMake 3.20+
- **Testing**: doctest 2.4+ via CTest
- **Dependencies**: Standard library only (no external libharu DLLs required)
- **External Services**: None (100% offline)

## 6. Folder Structure
```
14-PDF-Generator-App/
├── include/
│   └── PdfDocument.hpp
├── src/
│   ├── PdfDocument.cpp
│   └── main.cpp
├── tests/
│   └── test_pdf_generator.cpp
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

# Generate sample executive invoice PDF:
./build/PDFGeneratorApp --test-run
```

## 9. Testing Instructions
```bash
ctest --test-dir build --output-on-failure
```

## 10. Console Output Sample
```
=================================================================
                PDF DOCUMENT GENERATOR ENGINE                    
=================================================================
[Test-Run Mode] Generating sample PDF document (Executive Invoice)...
Generated valid PDF 1.4 document: sample_invoice.pdf
[Test-Run Mode] Completed successfully.
```

## 11. Known Limitations
- Current version focuses on standard 14 PostScript fonts (Helvetica); embedded TrueType/OpenType font parsing (TTF) requires FreeType.

## 12. Testing Status
- **Built**: Yes (Clean compilation under GCC 16.1.0 on Windows, 0 warnings).
- **Run**: Verified `--test-run` and verified generated `.pdf` document structure.
- **Tested**: 3 comprehensive test suites passing under CTest.
