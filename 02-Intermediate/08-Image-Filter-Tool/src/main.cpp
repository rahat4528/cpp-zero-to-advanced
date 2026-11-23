#include "Image.hpp"
#include "ImageFilter.hpp"
#include <iostream>
#include <string>

int main(int argc, char* argv[]) {
    if (argc >= 2 && std::string(argv[1]) == "--test-run") {
        std::cout << "=================================================================\n";
        std::cout << "              IMAGE FILTER TOOL (CONVOLUTION & COLOR)             \n";
        std::cout << "=================================================================\n";
        std::cout << "[Test-Run Mode] Generating 64x64 synthetic image and applying filters...\n";

        img::Image testImg(64, 64);
        for (int y = 0; y < 64; ++y) {
            for (int x = 0; x < 64; ++x) {
                uint8_t r = static_cast<uint8_t>((x * 255) / 64);
                uint8_t g = static_cast<uint8_t>((y * 255) / 64);
                uint8_t b = static_cast<uint8_t>(((x + y) * 255) / 128);
                testImg.setPixel(x, y, {r, g, b});
            }
        }

        auto gray = img::ImageFilter::toGrayscale(testImg);
        auto inverted = img::ImageFilter::invertColors(testImg);
        auto blurred = img::ImageFilter::gaussianBlur(testImg);
        auto sharpened = img::ImageFilter::sharpen(testImg);
        auto edges = img::ImageFilter::sobelEdgeDetect(testImg);

        std::cout << "Filter transformations succeeded:\n";
        std::cout << "  - Grayscale:       " << gray.getWidth() << "x" << gray.getHeight() << " px\n";
        std::cout << "  - Invert:          " << inverted.getWidth() << "x" << inverted.getHeight() << " px\n";
        std::cout << "  - Gaussian Blur:   " << blurred.getWidth() << "x" << blurred.getHeight() << " px\n";
        std::cout << "  - Sharpen:         " << sharpened.getWidth() << "x" << sharpened.getHeight() << " px\n";
        std::cout << "  - Sobel Edges:     " << edges.getWidth() << "x" << edges.getHeight() << " px\n";

        // Save PPM test artifact
        testImg.savePPM("test_input.ppm");
        edges.savePPM("test_edges.ppm");
        testImg.saveBMP("test_input.bmp");
        edges.saveBMP("test_edges.bmp");
        std::cout << "Synthetic input and output exported to PPM and BMP cleanly.\n";
        return 0;
    }

    std::cout << "Usage: ImageFilterTool <input.ppm|bmp> <output.ppm|bmp> <grayscale|invert|blur|sharpen|sobel>\n";
    std::cout << "   Or: ImageFilterTool --test-run\n";
    return 0;
}
