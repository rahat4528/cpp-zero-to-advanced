#include "HuffmanZipper.h"

#include <iomanip>
#include <iostream>
#include <string>

void printHelp() {
    std::cout << "Huffman File Zipper CLI\n"
              << "Usage:\n"
              << "  FileZipper --compress <input> --out <output.huf>   Compress file\n"
              << "  FileZipper --decompress <input.huf> --out <output> Decompress file\n"
              << "  FileZipper --help                                  Show help\n"
              << "Examples:\n"
              << "  FileZipper --compress document.txt --out document.huf\n"
              << "  FileZipper --decompress document.huf --out document_restored.txt\n";
}

int main(int argc, char* argv[]) {
    std::string mode;
    std::string inputPath;
    std::string outputPath;

    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "--help" || arg == "-h") {
            printHelp();
            return 0;
        } else if (arg == "--compress" && i + 1 < argc) {
            mode = "compress";
            inputPath = argv[++i];
        } else if (arg == "--decompress" && i + 1 < argc) {
            mode = "decompress";
            inputPath = argv[++i];
        } else if (arg == "--out" && i + 1 < argc) {
            outputPath = argv[++i];
        }
    }

    if (mode == "compress" && !inputPath.empty() && !outputPath.empty()) {
        file_zipper::CompressionStats stats;
        std::cout << "Compressing '" << inputPath << "' -> '" << outputPath << "'...\n";
        if (file_zipper::HuffmanZipper::compressFile(inputPath, outputPath, &stats)) {
            std::cout << std::fixed << std::setprecision(2);
            std::cout << "\n============================================\n"
                      << "          COMPRESSION SUCCESSFUL            \n"
                      << "============================================\n"
                      << "Original File Size   : " << stats.originalBytes << " bytes\n"
                      << "Compressed File Size : " << stats.compressedBytes << " bytes\n"
                      << "Compression Ratio    : " << stats.compressionRatio << " %\n"
                      << "Space Saved          : " << stats.spaceSavedPct << " %\n"
                      << "============================================\n";
            return 0;
        } else {
            std::cerr << "Error: Compression failed. Check input file path.\n";
            return 1;
        }
    } else if (mode == "decompress" && !inputPath.empty() && !outputPath.empty()) {
        std::cout << "Decompressing '" << inputPath << "' -> '" << outputPath << "'...\n";
        if (file_zipper::HuffmanZipper::decompressFile(inputPath, outputPath)) {
            std::cout << "Decompression completed successfully: '" << outputPath << "'\n";
            return 0;
        } else {
            std::cerr << "Error: Decompression failed. File may be corrupted or not a valid archive.\n";
            return 1;
        }
    }

    printHelp();
    return 0;
}
