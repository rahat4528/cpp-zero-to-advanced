#include "OCREngine.hpp"
#include <algorithm>
#include <cmath>

namespace ocr {

// Standard 8x8 font bit patterns
static const uint8_t GLYPH_0[8] = {0x3C, 0x66, 0x6E, 0x76, 0x66, 0x66, 0x3C, 0x00};
static const uint8_t GLYPH_1[8] = {0x18, 0x38, 0x18, 0x18, 0x18, 0x18, 0x7E, 0x00};
static const uint8_t GLYPH_2[8] = {0x3C, 0x66, 0x06, 0x1C, 0x30, 0x60, 0x7E, 0x00};
static const uint8_t GLYPH_3[8] = {0x3C, 0x66, 0x06, 0x1C, 0x06, 0x66, 0x3C, 0x00};
static const uint8_t GLYPH_4[8] = {0x0C, 0x1C, 0x34, 0x64, 0x7E, 0x04, 0x04, 0x00};
static const uint8_t GLYPH_5[8] = {0x7E, 0x60, 0x7C, 0x06, 0x06, 0x66, 0x3C, 0x00};
static const uint8_t GLYPH_6[8] = {0x3C, 0x60, 0x7C, 0x66, 0x66, 0x66, 0x3C, 0x00};
static const uint8_t GLYPH_7[8] = {0x7E, 0x06, 0x0C, 0x18, 0x30, 0x30, 0x30, 0x00};
static const uint8_t GLYPH_8[8] = {0x3C, 0x66, 0x66, 0x3C, 0x66, 0x66, 0x3C, 0x00};
static const uint8_t GLYPH_9[8] = {0x3C, 0x66, 0x66, 0x3E, 0x06, 0x0C, 0x38, 0x00};
static const uint8_t GLYPH_A[8] = {0x18, 0x3C, 0x66, 0x66, 0x7E, 0x66, 0x66, 0x00};
static const uint8_t GLYPH_B[8] = {0x7C, 0x66, 0x66, 0x7C, 0x66, 0x66, 0x7C, 0x00};
static const uint8_t GLYPH_C[8] = {0x3C, 0x66, 0x60, 0x60, 0x60, 0x66, 0x3C, 0x00};
static const uint8_t GLYPH_D[8] = {0x78, 0x6C, 0x66, 0x66, 0x66, 0x6C, 0x78, 0x00};
static const uint8_t GLYPH_E[8] = {0x7E, 0x60, 0x60, 0x7C, 0x60, 0x60, 0x7E, 0x00};
static const uint8_t GLYPH_F[8] = {0x7E, 0x60, 0x60, 0x7C, 0x60, 0x60, 0x60, 0x00};
static const uint8_t GLYPH_O[8] = {0x3C, 0x66, 0x66, 0x66, 0x66, 0x66, 0x3C, 0x00};
static const uint8_t GLYPH_P[8] = {0x7C, 0x66, 0x66, 0x7C, 0x60, 0x60, 0x60, 0x00};
static const uint8_t GLYPH_R[8] = {0x7C, 0x66, 0x66, 0x7C, 0x6C, 0x66, 0x66, 0x00};
static const uint8_t GLYPH_T[8] = {0x7E, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x00};

const uint8_t* OCREngine::getBuiltInFontGlyph(char c) {
    switch (c) {
        case '0': return GLYPH_0;
        case '1': return GLYPH_1;
        case '2': return GLYPH_2;
        case '3': return GLYPH_3;
        case '4': return GLYPH_4;
        case '5': return GLYPH_5;
        case '6': return GLYPH_6;
        case '7': return GLYPH_7;
        case '8': return GLYPH_8;
        case '9': return GLYPH_9;
        case 'A': return GLYPH_A;
        case 'B': return GLYPH_B;
        case 'C': return GLYPH_C;
        case 'D': return GLYPH_D;
        case 'E': return GLYPH_E;
        case 'F': return GLYPH_F;
        case 'O': return GLYPH_O;
        case 'P': return GLYPH_P;
        case 'R': return GLYPH_R;
        case 'T': return GLYPH_T;
        default:  return nullptr;
    }
}

OCREngine::OCREngine() {
    registerTemplate('0', GLYPH_0);
    registerTemplate('1', GLYPH_1);
    registerTemplate('2', GLYPH_2);
    registerTemplate('3', GLYPH_3);
    registerTemplate('4', GLYPH_4);
    registerTemplate('5', GLYPH_5);
    registerTemplate('6', GLYPH_6);
    registerTemplate('7', GLYPH_7);
    registerTemplate('8', GLYPH_8);
    registerTemplate('9', GLYPH_9);
    registerTemplate('A', GLYPH_A);
    registerTemplate('B', GLYPH_B);
    registerTemplate('C', GLYPH_C);
    registerTemplate('D', GLYPH_D);
    registerTemplate('E', GLYPH_E);
    registerTemplate('F', GLYPH_F);
    registerTemplate('O', GLYPH_O);
    registerTemplate('P', GLYPH_P);
    registerTemplate('R', GLYPH_R);
    registerTemplate('T', GLYPH_T);
}

void OCREngine::registerTemplate(char c, const uint8_t bitmap8x8[8]) {
    ImageRaster r(8, 8, 255);
    r.drawGlyph8x8(0, 0, bitmap8x8, 0);

    int minX = 8, maxX = -1, minY = 8, maxY = -1;
    for (int y = 0; y < 8; ++y) {
        for (int x = 0; x < 8; ++x) {
            if (r.getPixel(x, y) < 128) {
                minX = std::min(minX, x);
                maxX = std::max(maxX, x);
                minY = std::min(minY, y);
                maxY = std::max(maxY, y);
            }
        }
    }

    GlyphTemplate tmpl;
    tmpl.character = c;
    if (minX <= maxX && minY <= maxY) {
        BoundingBox box{minX, minY, maxX - minX + 1, maxY - minY + 1};
        tmpl.grid8x8 = r.extractNormalized8x8(box);
    } else {
        tmpl.grid8x8.assign(64, 0);
    }
    m_templates.push_back(std::move(tmpl));
}

RecognizedCharacter OCREngine::recognizeGlyph(const ImageRaster& raster, const BoundingBox& box) const {
    auto sampleGrid = raster.extractNormalized8x8(box);

    char bestChar = '?';
    int minDiff = 65;

    for (const auto& tmpl : m_templates) {
        int diff = 0;
        for (size_t i = 0; i < 64; ++i) {
            if (sampleGrid[i] != tmpl.grid8x8[i]) diff++;
        }
        if (diff < minDiff) {
            minDiff = diff;
            bestChar = tmpl.character;
        }
    }

    float confidence = 1.0f - (static_cast<float>(minDiff) / 64.0f);
    return {bestChar, confidence, box};
}

std::string OCREngine::recognizeText(const ImageRaster& raster) const {
    auto binarized = raster.binarizeOtsu();
    auto boxes = binarized.segmentConnectedComponents();

    if (boxes.empty()) return "";

    // Sort reading order: Top-to-bottom (Y), Left-to-right (X)
    std::sort(boxes.begin(), boxes.end(), [](const BoundingBox& a, const BoundingBox& b) {
        if (std::abs(a.y - b.y) > 10) return a.y < b.y;
        return a.x < b.x;
    });

    std::string text;
    int lastX = -1;

    for (const auto& box : boxes) {
        if (lastX != -1 && (box.x - lastX) > 4) {
            text += ' '; // Detect word break
        }
        auto recognized = recognizeGlyph(binarized, box);
        text += recognized.character;
        lastX = box.x + box.width;
    }

    return text;
}

} // namespace ocr
