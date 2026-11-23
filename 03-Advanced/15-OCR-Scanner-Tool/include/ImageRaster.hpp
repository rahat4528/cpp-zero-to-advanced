#pragma once

#include <vector>
#include <string>
#include <cstdint>

namespace ocr {

struct BoundingBox {
    int x{0};
    int y{0};
    int width{0};
    int height{0};
};

class ImageRaster {
public:
    ImageRaster(int width = 0, int height = 0, uint8_t initialColor = 255);

    [[nodiscard]] int getWidth() const noexcept { return m_width; }
    [[nodiscard]] int getHeight() const noexcept { return m_height; }
    [[nodiscard]] const std::vector<uint8_t>& getPixels() const noexcept { return m_pixels; }
    [[nodiscard]] uint8_t getPixel(int x, int y) const;
    void setPixel(int x, int y, uint8_t val);

    void drawRect(int x, int y, int w, int h, uint8_t color, bool filled = true);
    void drawGlyph8x8(int startX, int startY, const uint8_t glyph[8], uint8_t fgColor = 0);

    [[nodiscard]] ImageRaster binarizeOtsu() const;
    [[nodiscard]] std::vector<BoundingBox> segmentConnectedComponents() const;
    [[nodiscard]] std::vector<uint8_t> extractNormalized8x8(const BoundingBox& box) const;

private:
    int m_width{0};
    int m_height{0};
    std::vector<uint8_t> m_pixels;
};

} // namespace ocr
