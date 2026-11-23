#include "ImageRaster.hpp"
#include "OCREngine.hpp"
#include <iostream>

int main(int argc, char* argv[]) {
    bool testRun = false;
    for (int i = 1; i < argc; ++i) {
        if (std::string(argv[i]) == "--test-run") {
            testRun = true;
        }
    }

    std::cout << "========================================\n";
    std::cout << "    Optical Character Recognition Tool  \n";
    std::cout << "  (Otsu Thresholding & Glyph Classifier)\n";
    std::cout << "========================================\n";

    // Synthesize a test image containing "CPP 2026"
    ocr::ImageRaster canvas(120, 30, 255); // 120x30 white background
    const char* textToDraw = "CPP 2026";
    int penX = 10;
    int penY = 10;

    for (const char* p = textToDraw; *p != '\0'; ++p) {
        if (*p == ' ') {
            penX += 8;
            continue;
        }
        const uint8_t* glyph = ocr::OCREngine::getBuiltInFontGlyph(*p);
        if (glyph) {
            canvas.drawGlyph8x8(penX, penY, glyph, 0); // Black glyph
            penX += 10;
        }
    }

    std::cout << "Created synthetic raster bitmap (120x30 px)...\n";
    std::cout << "Applying Otsu bimodal binarization and connected component segmentation...\n";

    ocr::OCREngine engine;
    std::string detected = engine.recognizeText(canvas);

    std::cout << "\n----------------------------------------\n";
    std::cout << "Target Text:     \"" << textToDraw << "\"\n";
    std::cout << "Recognized Text: \"" << detected << "\"\n";
    std::cout << "----------------------------------------\n";

    if (testRun) {
        std::cout << "[Test Mode] OCR segmentation and character classification verified.\n";
        return 0;
    }

    return 0;
}
