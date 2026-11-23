#include "FaceDetector.hpp"
#include <algorithm>
#include <cmath>

namespace vision {

double DetectionRect::intersectionOverUnion(const DetectionRect& other) const {
    int x1 = std::max(x, other.x);
    int y1 = std::max(y, other.y);
    int x2 = std::min(x + width, other.x + other.width);
    int y2 = std::min(y + height, other.y + other.height);

    if (x2 <= x1 || y2 <= y1) return 0.0;

    double intersection = static_cast<double>((x2 - x1) * (y2 - y1));
    double unionArea = area() + other.area() - intersection;
    if (unionArea <= 0.0) return 0.0;
    return intersection / unionArea;
}

IntegralImage::IntegralImage(const GrayscaleRaster& raster)
    : m_width(raster.width), m_height(raster.height),
      m_integral((raster.width + 1) * (raster.height + 1), 0) {
    if (m_width <= 0 || m_height <= 0) return;

    for (int y = 0; y < m_height; ++y) {
        int64_t rowSum = 0;
        for (int x = 0; x < m_width; ++x) {
            rowSum += raster.pixels[y * m_width + x];
            m_integral[(y + 1) * (m_width + 1) + (x + 1)] =
                m_integral[y * (m_width + 1) + (x + 1)] + rowSum;
        }
    }
}

int64_t IntegralImage::getRectSum(int x, int y, int w, int h) const {
    if (x < 0 || y < 0 || x + w > m_width || y + h > m_height || w <= 0 || h <= 0) {
        return 0;
    }
    int stride = m_width + 1;
    int64_t a = m_integral[y * stride + x];
    int64_t b = m_integral[y * stride + (x + w)];
    int64_t c = m_integral[(y + h) * stride + x];
    int64_t d = m_integral[(y + h) * stride + (x + w)];

    return d - b - c + a;
}

FaceDetectorCascade::FaceDetectorCascade() {
    // Stage 1: Eye region darkness relative to forehead/cheeks (Horizontal Two-Rectangle)
    m_cascade.push_back({HaarFeatureType::TwoRectangleVertical, 3, 5, 18, 6, -15.0, 1.0});
    // Stage 2: Bridge of nose lightness relative to eyes (Horizontal Three-Rectangle)
    m_cascade.push_back({HaarFeatureType::ThreeRectangleHorizontal, 4, 7, 16, 6, 10.0, 1.2});
    // Stage 3: Mouth/lip contrast
    m_cascade.push_back({HaarFeatureType::TwoRectangleVertical, 5, 16, 14, 4, -8.0, 0.8});
}

void FaceDetectorCascade::addFeature(const HaarFeature& feature) {
    m_cascade.push_back(feature);
}

bool FaceDetectorCascade::evaluateWindow(const IntegralImage& ii, int x, int y, int size, double& outScore) const {
    double scale = static_cast<double>(size) / static_cast<double>(m_baseWindowSize);
    double accumulatedScore = 0.0;

    for (const auto& f : m_cascade) {
        int fx = x + static_cast<int>(std::round(f.x * scale));
        int fy = y + static_cast<int>(std::round(f.y * scale));
        int fw = static_cast<int>(std::round(f.width * scale));
        int fh = static_cast<int>(std::round(f.height * scale));

        double val = 0.0;
        if (f.type == HaarFeatureType::TwoRectangleVertical) {
            int halfH = fh / 2;
            int64_t top = ii.getRectSum(fx, fy, fw, halfH);
            int64_t bot = ii.getRectSum(fx, fy + halfH, fw, halfH);
            val = static_cast<double>(bot - top) / static_cast<double>(fw * halfH + 1);
        } else if (f.type == HaarFeatureType::ThreeRectangleHorizontal) {
            int thirdW = fw / 3;
            int64_t left = ii.getRectSum(fx, fy, thirdW, fh);
            int64_t mid = ii.getRectSum(fx + thirdW, fy, thirdW, fh);
            int64_t right = ii.getRectSum(fx + 2 * thirdW, fy, thirdW, fh);
            val = static_cast<double>(mid * 2 - left - right) / static_cast<double>(thirdW * fh + 1);
        } else {
            int halfW = fw / 2;
            int64_t left = ii.getRectSum(fx, fy, halfW, fh);
            int64_t right = ii.getRectSum(fx + halfW, fy, halfW, fh);
            val = static_cast<double>(right - left) / static_cast<double>(halfW * fh + 1);
        }

        if (val < f.threshold) {
            return false; // Early cascade rejection
        }
        accumulatedScore += val * f.weight;
    }

    outScore = accumulatedScore;
    return true;
}

std::vector<DetectionRect> FaceDetectorCascade::detectFaces(const GrayscaleRaster& image,
                                                           double scaleFactor,
                                                           int minNeighbors) const {
    std::vector<DetectionRect> rawDetections;
    IntegralImage ii(image);

    int maxDim = std::min(image.width, image.height);
    int stepSize = 4;

    for (double scale = 1.0; (m_baseWindowSize * scale) <= maxDim; scale *= scaleFactor) {
        int windowSize = static_cast<int>(std::round(m_baseWindowSize * scale));
        int step = std::max(2, static_cast<int>(stepSize * scale));

        for (int y = 0; y <= image.height - windowSize; y += step) {
            for (int x = 0; x <= image.width - windowSize; x += step) {
                double score = 0.0;
                if (evaluateWindow(ii, x, y, windowSize, score)) {
                    rawDetections.push_back({x, y, windowSize, windowSize, score});
                }
            }
        }
    }

    if (minNeighbors <= 0) return rawDetections;
    return applyNonMaxSuppression(rawDetections, 0.3);
}

std::vector<DetectionRect> FaceDetectorCascade::applyNonMaxSuppression(const std::vector<DetectionRect>& rects,
                                                                      double iouThreshold) {
    if (rects.empty()) return {};

    std::vector<DetectionRect> sorted = rects;
    std::sort(sorted.begin(), sorted.end(), [](const DetectionRect& a, const DetectionRect& b) {
        return a.confidence > b.confidence;
    });

    std::vector<DetectionRect> results;
    std::vector<bool> suppressed(sorted.size(), false);

    for (size_t i = 0; i < sorted.size(); ++i) {
        if (suppressed[i]) continue;
        results.push_back(sorted[i]);

        for (size_t j = i + 1; j < sorted.size(); ++j) {
            if (!suppressed[j]) {
                if (sorted[i].intersectionOverUnion(sorted[j]) > iouThreshold) {
                    suppressed[j] = true;
                }
            }
        }
    }
    return results;
}

} // namespace vision
