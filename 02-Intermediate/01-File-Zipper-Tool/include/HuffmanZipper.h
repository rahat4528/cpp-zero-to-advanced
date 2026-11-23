#pragma once

#include <cstdint>
#include <map>
#include <memory>
#include <string>
#include <vector>

namespace file_zipper {

struct CompressionStats {
    size_t originalBytes{0};
    size_t compressedBytes{0};
    double compressionRatio{0.0}; // (compressed / original) * 100
    double spaceSavedPct{0.0};
};

struct HuffmanNode {
    uint8_t byteValue{0};
    size_t frequency{0};
    std::shared_ptr<HuffmanNode> left;
    std::shared_ptr<HuffmanNode> right;

    HuffmanNode(uint8_t val, size_t freq) : byteValue(val), frequency(freq) {}
    HuffmanNode(size_t freq, std::shared_ptr<HuffmanNode> l, std::shared_ptr<HuffmanNode> r)
        : frequency(freq), left(std::move(l)), right(std::move(r)) {}

    bool isLeaf() const { return !left && !right; }
};

class BitWriter {
public:
    void writeBit(int bit);
    void writeByte(uint8_t byte);
    std::vector<uint8_t> flush();

private:
    std::vector<uint8_t> m_buffer;
    uint8_t m_currentByte{0};
    int m_bitCount{0};
};

class BitReader {
public:
    explicit BitReader(const std::vector<uint8_t>& data);
    int readBit();
    bool hasMoreBits() const;

private:
    const std::vector<uint8_t>& m_data;
    size_t m_byteIndex{0};
    int m_bitIndex{0};
};

class HuffmanZipper {
public:
    HuffmanZipper() = default;

    // In-memory compression and decompression
    static std::vector<uint8_t> compressBytes(const std::vector<uint8_t>& input);
    static std::vector<uint8_t> decompressBytes(const std::vector<uint8_t>& compressed);

    // File-based operations
    static bool compressFile(const std::string& inputPath, const std::string& outputPath, CompressionStats* stats = nullptr);
    static bool decompressFile(const std::string& inputPath, const std::string& outputPath);

    // Frequency calculation helper
    static std::map<uint8_t, size_t> calculateFrequencies(const std::vector<uint8_t>& data);
    static std::shared_ptr<HuffmanNode> buildTree(const std::map<uint8_t, size_t>& freqMap);
    static void generateCodes(const std::shared_ptr<HuffmanNode>& node,
                             const std::string& prefix,
                             std::map<uint8_t, std::string>& codeTable);
};

} // namespace file_zipper
