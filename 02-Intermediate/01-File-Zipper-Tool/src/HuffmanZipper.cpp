#include "HuffmanZipper.h"

#include <fstream>
#include <queue>

namespace file_zipper {

void BitWriter::writeBit(int bit) {
    if (bit) {
        m_currentByte |= (1 << (7 - m_bitCount));
    }
    m_bitCount++;
    if (m_bitCount == 8) {
        m_buffer.push_back(m_currentByte);
        m_currentByte = 0;
        m_bitCount = 0;
    }
}

void BitWriter::writeByte(uint8_t byte) {
    for (int i = 7; i >= 0; --i) {
        writeBit((byte >> i) & 1);
    }
}

std::vector<uint8_t> BitWriter::flush() {
    if (m_bitCount > 0) {
        m_buffer.push_back(m_currentByte);
        m_currentByte = 0;
        m_bitCount = 0;
    }
    return m_buffer;
}

BitReader::BitReader(const std::vector<uint8_t>& data) : m_data(data) {}

int BitReader::readBit() {
    if (m_byteIndex >= m_data.size()) return -1;

    int bit = (m_data[m_byteIndex] >> (7 - m_bitIndex)) & 1;
    m_bitIndex++;
    if (m_bitIndex == 8) {
        m_bitIndex = 0;
        m_byteIndex++;
    }
    return bit;
}

bool BitReader::hasMoreBits() const {
    return m_byteIndex < m_data.size();
}

std::map<uint8_t, size_t> HuffmanZipper::calculateFrequencies(const std::vector<uint8_t>& data) {
    std::map<uint8_t, size_t> freq;
    for (uint8_t b : data) {
        freq[b]++;
    }
    return freq;
}

struct NodeComparator {
    bool operator()(const std::shared_ptr<HuffmanNode>& a, const std::shared_ptr<HuffmanNode>& b) const {
        return a->frequency > b->frequency;
    }
};

std::shared_ptr<HuffmanNode> HuffmanZipper::buildTree(const std::map<uint8_t, size_t>& freqMap) {
    if (freqMap.empty()) return nullptr;

    std::priority_queue<std::shared_ptr<HuffmanNode>,
                        std::vector<std::shared_ptr<HuffmanNode>>,
                        NodeComparator> pq;

    for (const auto& [byteVal, count] : freqMap) {
        pq.push(std::make_shared<HuffmanNode>(byteVal, count));
    }

    if (pq.size() == 1) {
        auto single = pq.top();
        pq.pop();
        return std::make_shared<HuffmanNode>(single->frequency, single, nullptr);
    }

    while (pq.size() > 1) {
        auto left = pq.top(); pq.pop();
        auto right = pq.top(); pq.pop();
        auto parent = std::make_shared<HuffmanNode>(left->frequency + right->frequency, left, right);
        pq.push(parent);
    }

    return pq.top();
}

void HuffmanZipper::generateCodes(const std::shared_ptr<HuffmanNode>& node,
                                 const std::string& prefix,
                                 std::map<uint8_t, std::string>& codeTable) {
    if (!node) return;
    if (node->isLeaf()) {
        codeTable[node->byteValue] = prefix.empty() ? "0" : prefix;
        return;
    }
    generateCodes(node->left, prefix + "0", codeTable);
    generateCodes(node->right, prefix + "1", codeTable);
}

std::vector<uint8_t> HuffmanZipper::compressBytes(const std::vector<uint8_t>& input) {
    if (input.empty()) return {};

    auto freq = calculateFrequencies(input);
    auto root = buildTree(freq);

    std::map<uint8_t, std::string> codeTable;
    generateCodes(root, "", codeTable);

    // Serialization format:
    // [4 bytes]: Original uncompressed byte length (uint32_t)
    // [2 bytes]: Number of distinct symbols in table (uint16_t)
    // For each symbol:
    //   [1 byte]: symbol byte value
    //   [4 bytes]: symbol frequency (uint32_t)
    // [Bit stream]: Huffman encoded data
    std::vector<uint8_t> result;

    uint32_t origSize = static_cast<uint32_t>(input.size());
    for (int i = 0; i < 4; ++i) {
        result.push_back(static_cast<uint8_t>((origSize >> (i * 8)) & 0xFF));
    }

    uint16_t distinctCount = static_cast<uint16_t>(freq.size());
    result.push_back(static_cast<uint8_t>(distinctCount & 0xFF));
    result.push_back(static_cast<uint8_t>((distinctCount >> 8) & 0xFF));

    for (const auto& [b, count] : freq) {
        result.push_back(b);
        uint32_t c = static_cast<uint32_t>(count);
        for (int i = 0; i < 4; ++i) {
            result.push_back(static_cast<uint8_t>((c >> (i * 8)) & 0xFF));
        }
    }

    BitWriter bw;
    for (uint8_t b : input) {
        const std::string& code = codeTable[b];
        for (char bitChar : code) {
            bw.writeBit(bitChar == '1' ? 1 : 0);
        }
    }

    auto bitstream = bw.flush();
    result.insert(result.end(), bitstream.begin(), bitstream.end());
    return result;
}

std::vector<uint8_t> HuffmanZipper::decompressBytes(const std::vector<uint8_t>& compressed) {
    if (compressed.size() < 6) return {};

    uint32_t origSize = 0;
    for (int i = 0; i < 4; ++i) {
        origSize |= (static_cast<uint32_t>(compressed[i]) << (i * 8));
    }
    if (origSize == 0) return {};

    uint16_t distinctCount = static_cast<uint16_t>(compressed[4]) | (static_cast<uint16_t>(compressed[5]) << 8);
    size_t offset = 6;

    std::map<uint8_t, size_t> freq;
    for (uint16_t i = 0; i < distinctCount; ++i) {
        if (offset + 5 > compressed.size()) return {};
        uint8_t b = compressed[offset++];
        uint32_t count = 0;
        for (int j = 0; j < 4; ++j) {
            count |= (static_cast<uint32_t>(compressed[offset++]) << (j * 8));
        }
        freq[b] = count;
    }

    auto root = buildTree(freq);
    if (!root) return {};

    std::vector<uint8_t> bitData(compressed.begin() + offset, compressed.end());
    BitReader br(bitData);

    std::vector<uint8_t> output;
    output.reserve(origSize);

    auto curr = root;
    while (output.size() < origSize) {
        int bit = br.readBit();
        if (bit == -1) break;

        if (root->isLeaf()) {
            output.push_back(root->byteValue);
            continue;
        }

        curr = (bit == 1) ? curr->right : curr->left;
        if (!curr) break;

        if (curr->isLeaf()) {
            output.push_back(curr->byteValue);
            curr = root;
        }
    }

    return output;
}

bool HuffmanZipper::compressFile(const std::string& inputPath, const std::string& outputPath, CompressionStats* stats) {
    std::ifstream in(inputPath, std::ios::binary);
    if (!in.is_open()) return false;

    std::vector<uint8_t> data((std::istreambuf_iterator<char>(in)),
                               std::istreambuf_iterator<char>());
    in.close();

    auto compressed = compressBytes(data);

    std::ofstream out(outputPath, std::ios::binary);
    if (!out.is_open()) return false;
    out.write(reinterpret_cast<const char*>(compressed.data()), static_cast<std::streamsize>(compressed.size()));
    out.close();

    if (stats) {
        stats->originalBytes = data.size();
        stats->compressedBytes = compressed.size();
        if (stats->originalBytes > 0) {
            stats->compressionRatio = (static_cast<double>(stats->compressedBytes) / static_cast<double>(stats->originalBytes)) * 100.0;
            stats->spaceSavedPct = 100.0 - stats->compressionRatio;
        }
    }
    return true;
}

bool HuffmanZipper::decompressFile(const std::string& inputPath, const std::string& outputPath) {
    std::ifstream in(inputPath, std::ios::binary);
    if (!in.is_open()) return false;

    std::vector<uint8_t> compressed((std::istreambuf_iterator<char>(in)),
                                    std::istreambuf_iterator<char>());
    in.close();

    auto decompressed = decompressBytes(compressed);
    if (decompressed.empty() && !compressed.empty()) return false;

    std::ofstream out(outputPath, std::ios::binary);
    if (!out.is_open()) return false;
    out.write(reinterpret_cast<const char*>(decompressed.data()), static_cast<std::streamsize>(decompressed.size()));
    return true;
}

} // namespace file_zipper
