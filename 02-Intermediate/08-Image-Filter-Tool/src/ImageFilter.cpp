#include "ImageFilter.hpp"
#include <cmath>
#include <algorithm>

namespace img {

static inline uint8_t clampToByte(float val) {
    if (val < 0.0f) return 0;
    if (val > 255.0f) return 255;
    return static_cast<uint8_t>(std::round(val));
}

Image ImageFilter::toGrayscale(const Image& src) {
    Image dst(src.getWidth(), src.getHeight());
    for (int y = 0; y < src.getHeight(); ++y) {
        for (int x = 0; x < src.getWidth(); ++x) {
            Pixel p = src.getPixel(x, y);
            // Standard NTSC luminance weights
            auto lum = clampToByte(0.299f * p.r + 0.587f * p.g + 0.114f * p.b);
            dst.setPixel(x, y, {lum, lum, lum});
        }
    }
    return dst;
}

Image ImageFilter::invertColors(const Image& src) {
    Image dst(src.getWidth(), src.getHeight());
    for (int y = 0; y < src.getHeight(); ++y) {
        for (int x = 0; x < src.getWidth(); ++x) {
            Pixel p = src.getPixel(x, y);
            dst.setPixel(x, y, {
                static_cast<uint8_t>(255 - p.r),
                static_cast<uint8_t>(255 - p.g),
                static_cast<uint8_t>(255 - p.b)
            });
        }
    }
    return dst;
}

Image ImageFilter::adjustBrightness(const Image& src, int delta) {
    Image dst(src.getWidth(), src.getHeight());
    for (int y = 0; y < src.getHeight(); ++y) {
        for (int x = 0; x < src.getWidth(); ++x) {
            Pixel p = src.getPixel(x, y);
            dst.setPixel(x, y, {
                clampToByte(static_cast<float>(p.r + delta)),
                clampToByte(static_cast<float>(p.g + delta)),
                clampToByte(static_cast<float>(p.b + delta))
            });
        }
    }
    return dst;
}

Image ImageFilter::applyKernel3x3(const Image& src, const std::vector<float>& kernel, float divisor, float bias) {
    if (kernel.size() != 9) return src;

    int w = src.getWidth();
    int h = src.getHeight();
    Image dst(w, h);

    for (int y = 0; y < h; ++y) {
        for (int x = 0; x < w; ++x) {
            float rSum = 0.0f;
            float gSum = 0.0f;
            float bSum = 0.0f;

            int kIdx = 0;
            for (int ky = -1; ky <= 1; ++ky) {
                for (int kx = -1; kx <= 1; ++kx) {
                    int sampleX = std::clamp(x + kx, 0, w - 1);
                    int sampleY = std::clamp(y + ky, 0, h - 1);
                    Pixel p = src.getPixel(sampleX, sampleY);

                    float kVal = kernel[kIdx++];
                    rSum += p.r * kVal;
                    gSum += p.g * kVal;
                    bSum += p.b * kVal;
                }
            }

            dst.setPixel(x, y, {
                clampToByte((rSum / divisor) + bias),
                clampToByte((gSum / divisor) + bias),
                clampToByte((bSum / divisor) + bias)
            });
        }
    }
    return dst;
}

Image ImageFilter::gaussianBlur(const Image& src) {
    std::vector<float> kernel = {
        1.0f, 2.0f, 1.0f,
        2.0f, 4.0f, 2.0f,
        1.0f, 2.0f, 1.0f
    };
    return applyKernel3x3(src, kernel, 16.0f, 0.0f);
}

Image ImageFilter::sharpen(const Image& src) {
    std::vector<float> kernel = {
         0.0f, -1.0f,  0.0f,
        -1.0f,  5.0f, -1.0f,
         0.0f, -1.0f,  0.0f
    };
    return applyKernel3x3(src, kernel, 1.0f, 0.0f);
}

Image ImageFilter::sobelEdgeDetect(const Image& src) {
    Image gray = toGrayscale(src);
    int w = gray.getWidth();
    int h = gray.getHeight();
    Image dst(w, h);

    const float sobelX[9] = {
        -1.0f, 0.0f, 1.0f,
        -2.0f, 0.0f, 2.0f,
        -1.0f, 0.0f, 1.0f
    };

    const float sobelY[9] = {
        -1.0f, -2.0f, -1.0f,
         0.0f,  0.0f,  0.0f,
         1.0f,  2.0f,  1.0f
    };

    for (int y = 0; y < h; ++y) {
        for (int x = 0; x < w; ++x) {
            float gx = 0.0f;
            float gy = 0.0f;

            int kIdx = 0;
            for (int ky = -1; ky <= 1; ++ky) {
                for (int kx = -1; kx <= 1; ++kx) {
                    int sampleX = std::clamp(x + kx, 0, w - 1);
                    int sampleY = std::clamp(y + ky, 0, h - 1);
                    float val = static_cast<float>(gray.getPixel(sampleX, sampleY).r);

                    gx += val * sobelX[kIdx];
                    gy += val * sobelY[kIdx];
                    kIdx++;
                }
            }

            float mag = std::sqrt(gx * gx + gy * gy);
            uint8_t edgeVal = clampToByte(mag);
            dst.setPixel(x, y, {edgeVal, edgeVal, edgeVal});
        }
    }
    return dst;
}

} // namespace img
