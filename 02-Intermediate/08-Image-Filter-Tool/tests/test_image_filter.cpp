#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>
#include "Image.hpp"
#include "ImageFilter.hpp"
#include <filesystem>

TEST_CASE("Image Raster & Pixel Operations") {
    img::Image img(10, 10, {255, 0, 0});
    CHECK(img.getWidth() == 10);
    CHECK(img.getHeight() == 10);
    CHECK(img.isValid() == true);

    CHECK(img.getPixel(0, 0) == img::Pixel{255, 0, 0});
    img.setPixel(5, 5, {0, 255, 0});
    CHECK(img.getPixel(5, 5) == img::Pixel{0, 255, 0});
}

TEST_CASE("ImageFilter Transforms") {
    img::Image img(4, 4, {100, 150, 200});

    SUBCASE("Grayscale Transform") {
        auto gray = img::ImageFilter::toGrayscale(img);
        auto p = gray.getPixel(0, 0);
        CHECK(p.r == p.g);
        CHECK(p.g == p.b);
        // 0.299*100 + 0.587*150 + 0.114*200 = 29.9 + 88.05 + 22.8 = 140.75 -> 141
        CHECK(p.r == 141);
    }

    SUBCASE("Color Inversion") {
        auto inv = img::ImageFilter::invertColors(img);
        auto p = inv.getPixel(0, 0);
        CHECK(p.r == 155);
        CHECK(p.g == 105);
        CHECK(p.b == 55);
    }

    SUBCASE("Brightness Adjustment") {
        auto bright = img::ImageFilter::adjustBrightness(img, 50);
        auto p = bright.getPixel(0, 0);
        CHECK(p.r == 150);
        CHECK(p.g == 200);
        CHECK(p.b == 250);

        auto clamped = img::ImageFilter::adjustBrightness(img, 200);
        auto cp = clamped.getPixel(0, 0);
        CHECK(cp.b == 255);
    }

    SUBCASE("Convolution Blur & Edge Detection") {
        auto blur = img::ImageFilter::gaussianBlur(img);
        CHECK(blur.getWidth() == 4);
        CHECK(blur.getHeight() == 4);

        auto sobel = img::ImageFilter::sobelEdgeDetect(img);
        // Uniform color image has zero gradients
        auto sp = sobel.getPixel(2, 2);
        CHECK(sp.r == 0);
        CHECK(sp.g == 0);
        CHECK(sp.b == 0);
    }
}

TEST_CASE("Image File Formats Roundtrip") {
    img::Image original(8, 8);
    for (int y = 0; y < 8; ++y) {
        for (int x = 0; x < 8; ++x) {
            original.setPixel(x, y, {
                static_cast<uint8_t>(x * 30),
                static_cast<uint8_t>(y * 30),
                static_cast<uint8_t>((x + y) * 15)
            });
        }
    }

    SUBCASE("PPM Roundtrip") {
        std::string ppmPath = "temp_test_roundtrip.ppm";
        CHECK(original.savePPM(ppmPath) == true);

        img::Image loaded;
        CHECK(loaded.loadPPM(ppmPath) == true);
        CHECK(loaded.getWidth() == 8);
        CHECK(loaded.getHeight() == 8);
        CHECK(loaded.getPixel(3, 3) == original.getPixel(3, 3));

        std::filesystem::remove(ppmPath);
    }

    SUBCASE("BMP Roundtrip") {
        std::string bmpPath = "temp_test_roundtrip.bmp";
        CHECK(original.saveBMP(bmpPath) == true);

        img::Image loaded;
        CHECK(loaded.loadBMP(bmpPath) == true);
        CHECK(loaded.getWidth() == 8);
        CHECK(loaded.getHeight() == 8);
        CHECK(loaded.getPixel(3, 3) == original.getPixel(3, 3));

        std::filesystem::remove(bmpPath);
    }
}
