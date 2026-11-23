#include "Image.hpp"
#include <fstream>
#include <sstream>
#include <iostream>
#include <cstring>
#include <algorithm>

namespace img {

Image::Image(int width, int height, Pixel fill)
    : m_width(width), m_height(height), m_pixels(width * height, fill) {}

int Image::getWidth() const { return m_width; }
int Image::getHeight() const { return m_height; }
bool Image::isValid() const { return m_width > 0 && m_height > 0 && !m_pixels.empty(); }

Pixel Image::getPixel(int x, int y) const {
    if (x < 0 || x >= m_width || y < 0 || y >= m_height) {
        return {0, 0, 0};
    }
    return m_pixels[y * m_width + x];
}

void Image::setPixel(int x, int y, Pixel p) {
    if (x >= 0 && x < m_width && y >= 0 && y < m_height) {
        m_pixels[y * m_width + x] = p;
    }
}

bool Image::loadPPM(const std::string& path) {
    std::ifstream in(path, std::ios::binary);
    if (!in.is_open()) return false;

    std::string magic;
    in >> magic;
    if (magic != "P6") return false;

    int w = 0, h = 0, maxVal = 0;
    in >> w >> h >> maxVal;
    in.get(); // Skip trailing newline or whitespace

    if (w <= 0 || h <= 0 || maxVal != 255) return false;

    m_width = w;
    m_height = h;
    m_pixels.resize(w * h);

    std::vector<uint8_t> rawBuffer(w * h * 3);
    in.read(reinterpret_cast<char*>(rawBuffer.data()), rawBuffer.size());
    if (in.gcount() != static_cast<std::streamsize>(rawBuffer.size())) return false;

    for (int i = 0; i < w * h; ++i) {
        m_pixels[i].r = rawBuffer[i * 3 + 0];
        m_pixels[i].g = rawBuffer[i * 3 + 1];
        m_pixels[i].b = rawBuffer[i * 3 + 2];
    }
    return true;
}

bool Image::savePPM(const std::string& path) const {
    if (!isValid()) return false;

    std::ofstream out(path, std::ios::binary);
    if (!out.is_open()) return false;

    out << "P6\n" << m_width << " " << m_height << "\n255\n";

    std::vector<uint8_t> rawBuffer(m_width * m_height * 3);
    for (int i = 0; i < m_width * m_height; ++i) {
        rawBuffer[i * 3 + 0] = m_pixels[i].r;
        rawBuffer[i * 3 + 1] = m_pixels[i].g;
        rawBuffer[i * 3 + 2] = m_pixels[i].b;
    }

    out.write(reinterpret_cast<const char*>(rawBuffer.data()), rawBuffer.size());
    return out.good();
}

#pragma pack(push, 1)
struct BMPHeader {
    uint16_t fileType{0x4D42}; // 'BM'
    uint32_t fileSize{0};
    uint16_t reserved1{0};
    uint16_t reserved2{0};
    uint32_t offsetData{54};
    uint32_t headerSize{40};
    int32_t  width{0};
    int32_t  height{0};
    uint16_t planes{1};
    uint16_t bitCount{24};
    uint32_t compression{0};
    uint32_t sizeImage{0};
    int32_t  xPelsPerMeter{0};
    int32_t  yPelsPerMeter{0};
    uint32_t clrUsed{0};
    uint32_t clrImportant{0};
};
#pragma pack(pop)

bool Image::loadBMP(const std::string& path) {
    std::ifstream in(path, std::ios::binary);
    if (!in.is_open()) return false;

    BMPHeader header;
    in.read(reinterpret_cast<char*>(&header), sizeof(header));
    if (header.fileType != 0x4D42 || header.bitCount != 24 || header.compression != 0) {
        return false;
    }

    m_width = header.width;
    m_height = std::abs(header.height);
    bool flipY = (header.height > 0); // BMPs are usually stored bottom-to-top

    m_pixels.resize(m_width * m_height);

    int rowPadding = (4 - (m_width * 3) % 4) % 4;
    in.seekg(header.offsetData, std::ios::beg);

    std::vector<uint8_t> rowBuffer(m_width * 3 + rowPadding);

    for (int y = 0; y < m_height; ++y) {
        in.read(reinterpret_cast<char*>(rowBuffer.data()), rowBuffer.size());
        int targetY = flipY ? (m_height - 1 - y) : y;

        for (int x = 0; x < m_width; ++x) {
            // BMP pixel order is B, G, R
            Pixel p;
            p.b = rowBuffer[x * 3 + 0];
            p.g = rowBuffer[x * 3 + 1];
            p.r = rowBuffer[x * 3 + 2];
            setPixel(x, targetY, p);
        }
    }
    return true;
}

bool Image::saveBMP(const std::string& path) const {
    if (!isValid()) return false;

    std::ofstream out(path, std::ios::binary);
    if (!out.is_open()) return false;

    int rowPadding = (4 - (m_width * 3) % 4) % 4;
    uint32_t rowSize = m_width * 3 + rowPadding;
    uint32_t imageSize = rowSize * m_height;

    BMPHeader header;
    header.fileSize = 54 + imageSize;
    header.width = m_width;
    header.height = m_height; // Positive means bottom-to-top
    header.sizeImage = imageSize;

    out.write(reinterpret_cast<const char*>(&header), sizeof(header));

    std::vector<uint8_t> rowBuffer(rowSize, 0);
    for (int y = m_height - 1; y >= 0; --y) {
        for (int x = 0; x < m_width; ++x) {
            Pixel p = getPixel(x, y);
            rowBuffer[x * 3 + 0] = p.b;
            rowBuffer[x * 3 + 1] = p.g;
            rowBuffer[x * 3 + 2] = p.r;
        }
        out.write(reinterpret_cast<const char*>(rowBuffer.data()), rowSize);
    }
    return out.good();
}

} // namespace img
