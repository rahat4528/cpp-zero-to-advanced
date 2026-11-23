#pragma once

#include "ImageRaster.hpp"
#include <string>
#include <vector>
#include <map>

namespace ocr {

struct RecognizedCharacter {
    char character{' '};
    float confidence{0.0f};
    BoundingBox box;
};

class OCREngine {
public:
    OCREngine();

    void registerTemplate(char c, const uint8_t bitmap8x8[8]);
    [[nodiscard]] RecognizedCharacter recognizeGlyph(const ImageRaster& raster, const BoundingBox& box) const;
    [[nodiscard]] std::string recognizeText(const ImageRaster& raster) const;

    [[nodiscard]] static const uint8_t* getBuiltInFontGlyph(char c);

private:
    struct GlyphTemplate {
        char character;
        std::vector<uint8_t> grid8x8; // 64 values (0 or 1)
    };
    std::vector<GlyphTemplate> m_templates;
};

} // namespace ocr
