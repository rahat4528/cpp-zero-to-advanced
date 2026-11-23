#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>
#include "ImageRaster.hpp"
#include "OCREngine.hpp"

using namespace ocr;

TEST_SUITE("Optical Character Recognition Engine") {
    TEST_CASE("ImageRaster Otsu Binarization") {
        ImageRaster raster(10, 10, 200); // Light gray
        raster.drawRect(2, 2, 4, 4, 30); // Dark square

        auto binarized = raster.binarizeOtsu();
        CHECK(binarized.getPixel(0, 0) == 255); // Background white
        CHECK(binarized.getPixel(3, 3) == 0);   // Foreground black
    }

    TEST_CASE("Connected Component Segmentation") {
        ImageRaster raster(40, 20, 255);
        raster.drawRect(5, 5, 6, 6, 0);   // Blob 1
        raster.drawRect(20, 5, 8, 8, 0);  // Blob 2

        auto boxes = raster.segmentConnectedComponents();
        REQUIRE(boxes.size() == 2);
        CHECK(boxes[0].width >= 5);
        CHECK(boxes[1].width >= 7);
    }

    TEST_CASE("Glyph Recognition with Synthetic Font Patterns") {
        ImageRaster raster(40, 16, 255);
        const uint8_t* glyphC = OCREngine::getBuiltInFontGlyph('C');
        const uint8_t* glyph1 = OCREngine::getBuiltInFontGlyph('1');
        REQUIRE(glyphC != nullptr);
        REQUIRE(glyph1 != nullptr);

        raster.drawGlyph8x8(4, 4, glyphC, 0);
        raster.drawGlyph8x8(16, 4, glyph1, 0);

        OCREngine engine;
        std::string text = engine.recognizeText(raster);
        CHECK(text.find('C') != std::string::npos);
        CHECK(text.find('1') != std::string::npos);
    }
}
