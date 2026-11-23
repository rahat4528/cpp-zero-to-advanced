#pragma once

#include <vector>
#include <string>
#include <cstdint>

namespace img {

struct Pixel {
    uint8_t r{0};
    uint8_t g{0};
    uint8_t b{0};

    bool operator==(const Pixel& other) const {
        return r == other.r && g == other.g && b == other.b;
    }
};

class Image {
public:
    Image() = default;
    Image(int width, int height, Pixel fill = {0, 0, 0});

    [[nodiscard]] int getWidth() const;
    [[nodiscard]] int getHeight() const;
    [[nodiscard]] bool isValid() const;

    [[nodiscard]] Pixel getPixel(int x, int y) const;
    void setPixel(int x, int y, Pixel p);

    // I/O: Netpbm binary PPM (P6) and 24-bit uncompressed BMP
    bool loadPPM(const std::string& path);
    bool savePPM(const std::string& path) const;

    bool loadBMP(const std::string& path);
    bool saveBMP(const std::string& path) const;

private:
    int m_width{0};
    int m_height{0};
    std::vector<Pixel> m_pixels;
};

} // namespace img
