#pragma once

#include "Image.hpp"
#include <vector>

namespace img {

class ImageFilter {
public:
    // Core transforms
    static Image toGrayscale(const Image& src);
    static Image invertColors(const Image& src);
    static Image adjustBrightness(const Image& src, int delta); // -255 to +255

    // 3x3 Convolution matrix
    static Image applyKernel3x3(const Image& src, const std::vector<float>& kernel, float divisor = 1.0f, float bias = 0.0f);

    // Convolution presets
    static Image gaussianBlur(const Image& src);
    static Image sharpen(const Image& src);
    static Image sobelEdgeDetect(const Image& src);
};

} // namespace img
