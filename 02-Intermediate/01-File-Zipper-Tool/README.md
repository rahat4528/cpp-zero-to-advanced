# 01-File-Zipper-Tool

A lossless binary File Compression and Decompression Tool implementing Canonical Huffman Coding and bitstream serialization built with modern C++17.

---

## 1. Project Overview
File Zipper Tool is a standalone file archiver and compressor. It constructs optimal binary prefix trees from symbol frequency histograms, packs variable-length bit codes into custom binary streams, preserves frequency metadata in custom archive headers, and restores files with byte-for-byte fidelity.

## 2. Problem Statement
Lossless data compression requires building prefix-free codes where frequent characters receive shorter bit sequences. Implementing a production-quality compressor in C++ requires custom bit-level stream I/O (`BitWriter` and `BitReader`), priority-queue min-heap tree synthesis, and deterministic header serialization.

## 3. Objectives
- Perform byte frequency analysis across raw 8-bit byte streams.
- Construct Huffman trees using `std::priority_queue` min-heaps.
- Implement low-level `BitWriter` and `BitReader` classes to write packed sub-byte bitstreams.
- Serialize tree headers and decode bit streams with 100% lossless fidelity.

## 4. Features
- **Lossless Decompression**: Guarantees byte-for-byte exact file reconstruction.
- **Bit-Level Serialization**: Packs variable bit length codes into compact 8-bit octets.
- **Space Savings Metrics**: Computes original vs compressed sizes, compression ratio, and space saved percentage.
- **Scriptable CLI**: Non-interactive compression via `--compress` and `--decompress`.

## 5. Technology Stack
- **Language**: C++17 (`std::priority_queue`, `std::map`, `std::vector`, `std::shared_ptr`, `std::fstream`, `uint8_t`)
- **Algorithm**: Canonical Huffman Coding. *Rationale*: Selected to demonstrate fundamental binary tree algorithms and bitstream manipulation cleanly without requiring external dynamic libraries.
- **Build System**: CMake 3.20+
- **Testing Framework**: doctest with CTest
- **Database Needed**: No. Operates on filesystem binary files.
- **Internet / External Service Needed**: No.

## 6. Folder Structure
```
01-File-Zipper-Tool/
├── include/
│   └── HuffmanZipper.h          # BitWriter, BitReader, HuffmanNode, and HuffmanZipper APIs
├── src/
│   ├── HuffmanZipper.cpp        # Tree construction, bitstream packing, compression
│   └── main.cpp                 # CLI driver
├── tests/
│   └── test_file_zipper.cpp     # doctest unit test suite
├── CMakeLists.txt               # CMake build configuration
├── .gitignore                   # Build ignore rules
└── README.md                    # Project documentation
```

## 7. Installation / Prerequisites
- GCC 11+, Clang 14+, or MSVC 2022
- CMake 3.20+ and Ninja (or Make)

## 8. Build and Run Instructions

### Windows (PowerShell)
```powershell
cmake -S . -B build -G Ninja
cmake --build build

# Compress a file
.\build\FileZipper.exe --compress document.txt --out document.huf

# Decompress a file
.\build\FileZipper.exe --decompress document.huf --out document_restored.txt
```

### Linux / macOS
```bash
cmake -S . -B build -G Ninja
cmake --build build

./build/FileZipper --compress input.txt --out input.huf
./build/FileZipper --decompress input.huf --out output.txt
```

## 9. Testing Instructions
Run tests with CTest:
```bash
ctest --test-dir build --output-on-failure
```

## 10. Demo / Console Output Sample
```
$ ./build/FileZipper --compress sample.txt --out sample.huf
Compressing 'sample.txt' -> 'sample.huf'...

============================================
          COMPRESSION SUCCESSFUL            
============================================
Original File Size   : 1000 bytes
Compressed File Size : 158 bytes
Compression Ratio    : 15.80 %
Space Saved          : 84.20 %
============================================
```

## 11. Known Limitations
- Static two-pass Huffman coding (does not perform sliding-window LZ77 dictionary compression).
- Very small files (< 50 bytes) may have headers larger than the compressed payload due to frequency table overhead.

## 12. Testing Status
- **Built**: Yes, compiled cleanly with GCC 16.1.0 using `-Wall -Wextra -Wpedantic`.
- **Run**: Yes, verified bitstream roundtrips and file-to-file compression/decompression.
- **Tested**: Yes, 5 unit test suites passing via CTest.
- **Hardware/Credentials Limitations**: None.
