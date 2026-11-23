#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest/doctest.h"
#include "HuffmanZipper.h"

#include <cstdio>
#include <string>

using namespace file_zipper;

TEST_CASE("BitWriter and BitReader roundtrip") {
    BitWriter bw;
    bw.writeBit(1);
    bw.writeBit(0);
    bw.writeBit(1);
    bw.writeBit(1);
    bw.writeBit(0);
    bw.writeBit(0);
    bw.writeBit(1);
    bw.writeBit(0); // 10110010 = 0xB2 = 178
    bw.writeBit(1); // 1 in second byte
    auto data = bw.flush();

    REQUIRE(data.size() == 2);
    CHECK(data[0] == 0xB2);
    CHECK(data[1] == 0x80);

    BitReader br(data);
    CHECK(br.readBit() == 1);
    CHECK(br.readBit() == 0);
    CHECK(br.readBit() == 1);
    CHECK(br.readBit() == 1);
    CHECK(br.readBit() == 0);
    CHECK(br.readBit() == 0);
    CHECK(br.readBit() == 1);
    CHECK(br.readBit() == 0);
    CHECK(br.readBit() == 1);
}

TEST_CASE("In-memory string lossless compression roundtrip") {
    std::string text = "TO BE OR NOT TO BE THAT IS THE QUESTION WHETHER TIS NOBLER IN THE MIND TO SUFFER";
    std::vector<uint8_t> original(text.begin(), text.end());

    auto compressed = HuffmanZipper::compressBytes(original);
    REQUIRE(!compressed.empty());

    auto decompressed = HuffmanZipper::decompressBytes(compressed);
    REQUIRE(decompressed.size() == original.size());
    CHECK(decompressed == original);
}

TEST_CASE("Highly repetitive data achieves substantial compression") {
    // 1000 identical bytes
    std::vector<uint8_t> data(1000, 'A');
    auto compressed = HuffmanZipper::compressBytes(data);

    // Compressed format (header + bitstream) should be much smaller than 1000 bytes
    CHECK(compressed.size() < 200);

    auto restored = HuffmanZipper::decompressBytes(compressed);
    CHECK(restored == data);
}

TEST_CASE("Empty data boundary") {
    std::vector<uint8_t> empty;
    auto compressed = HuffmanZipper::compressBytes(empty);
    CHECK(compressed.empty());

    auto decompressed = HuffmanZipper::decompressBytes(compressed);
    CHECK(decompressed.empty());
}

TEST_CASE("File-to-file compression roundtrip") {
    const std::string originalFile = "temp_test_uncompressed.txt";
    const std::string compressedFile = "temp_test_archive.huf";
    const std::string restoredFile = "temp_test_restored.txt";

    // Write original file
    {
        std::ofstream out(originalFile);
        out << "The quick brown fox jumps over the lazy dog repeatedly. "
            << "Huffman coding is an optimal prefix code algorithm used for lossless data compression.";
    }

    CompressionStats stats;
    CHECK(HuffmanZipper::compressFile(originalFile, compressedFile, &stats) == true);
    CHECK(stats.originalBytes > 0);
    CHECK(stats.compressedBytes > 0);

    CHECK(HuffmanZipper::decompressFile(compressedFile, restoredFile) == true);

    // Read back and compare
    std::ifstream inOrig(originalFile, std::ios::binary);
    std::string origStr((std::istreambuf_iterator<char>(inOrig)), std::istreambuf_iterator<char>());

    std::ifstream inRest(restoredFile, std::ios::binary);
    std::string restStr((std::istreambuf_iterator<char>(inRest)), std::istreambuf_iterator<char>());

    CHECK(origStr == restStr);

    std::remove(originalFile.c_str());
    std::remove(compressedFile.c_str());
    std::remove(restoredFile.c_str());
}
