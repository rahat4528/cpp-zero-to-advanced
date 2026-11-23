#pragma once

#include <vector>
#include <string>
#include <cstdint>

namespace vision {

struct DetectionRect {
    int x{0};
    int y{0};
    int width{0};
    int height{0};
    double confidence{0.0};

    [[nodiscard]] double area() const {
        return static_cast<double>(width * height);
    }

    [[nodiscard]] double intersectionOverUnion(const DetectionRect& other) const;
};

struct GrayscaleRaster {
    int width{0};
    int height{0};
    std::vector<uint8_t> pixels; // Row-major 8-bit luminance
};

class IntegralImage {
public:
    IntegralImage() = default;
    explicit IntegralImage(const GrayscaleRaster& raster);

    [[nodiscard]] int getWidth() const { return m_width; }
    [[nodiscard]] int getHeight() const { return m_height; }

    // Evaluates sum of rectangular region [x, y, w, h] in O(1)
    [[nodiscard]] int64_t getRectSum(int x, int y, int w, int h) const;

private:
    int m_width{0};
    int m_height{0};
    std::vector<int64_t> m_integral; // (width + 1) * (height + 1)
};

enum class HaarFeatureType {
    TwoRectangleHorizontal, // [ - | + ]
    TwoRectangleVertical,   // [ - ] / [ + ]
    ThreeRectangleHorizontal, // [ - | + | - ]
    FourRectangleDiagonal     // [ - | + ] / [ + | - ]
};

struct HaarFeature {
    HaarFeatureType type;
    int x;
    int y;
    int width;
    int height;
    double threshold;
    double weight;
};

class FaceDetectorCascade {
public:
    FaceDetectorCascade();

    void addFeature(const HaarFeature& feature);
    [[nodiscard]] std::vector<DetectionRect> detectFaces(const GrayscaleRaster& image,
                                                         double scaleFactor = 1.2,
                                                         int minNeighbors = 1) const;

    static std::vector<DetectionRect> applyNonMaxSuppression(const std::vector<DetectionRect>& rects,
                                                            double iouThreshold = 0.3);

private:
    [[nodiscard]] bool evaluateWindow(const IntegralImage& ii, int x, int y, int size, double& outScore) const;

    int m_baseWindowSize{24};
    std::vector<HaarFeature> m_cascade;
};

} // namespace vision
