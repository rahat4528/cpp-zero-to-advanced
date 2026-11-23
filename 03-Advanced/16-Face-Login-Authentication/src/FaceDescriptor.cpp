#include "FaceDescriptor.hpp"
#include <cmath>
#include <algorithm>

namespace face_auth {

FaceImage FaceDescriptor::resizeBilinear(const FaceImage& src, int targetW, int targetH) {
    FaceImage dst;
    dst.width = targetW;
    dst.height = targetH;
    dst.grayscale.resize(static_cast<size_t>(targetW * targetH));

    if (src.width <= 0 || src.height <= 0) return dst;

    float xRatio = static_cast<float>(src.width - 1) / static_cast<float>(targetW > 1 ? targetW - 1 : 1);
    float yRatio = static_cast<float>(src.height - 1) / static_cast<float>(targetH > 1 ? targetH - 1 : 1);

    for (int y = 0; y < targetH; ++y) {
        for (int x = 0; x < targetW; ++x) {
            float gx = static_cast<float>(x) * xRatio;
            float gy = static_cast<float>(y) * yRatio;
            int gxi = static_cast<int>(gx);
            int gyi = static_cast<int>(gy);
            float dx = gx - static_cast<float>(gxi);
            float dy = gy - static_cast<float>(gyi);

            float c00 = static_cast<float>(src.getPixel(gxi, gyi));
            float c10 = static_cast<float>(src.getPixel(gxi + 1, gyi));
            float c01 = static_cast<float>(src.getPixel(gxi, gyi + 1));
            float c11 = static_cast<float>(src.getPixel(gxi + 1, gyi + 1));

            float val = c00 * (1.0f - dx) * (1.0f - dy) +
                        c10 * dx * (1.0f - dy) +
                        c01 * (1.0f - dx) * dy +
                        c11 * dx * dy;

            dst.setPixel(x, y, static_cast<uint8_t>(std::clamp(val, 0.0f, 255.0f)));
        }
    }
    return dst;
}

std::vector<float> FaceDescriptor::extractLBPFeatures(const FaceImage& face) {
    FaceImage norm = resizeBilinear(face, NORM_SIZE, NORM_SIZE);

    // Divide 32x32 into 4x4 grid of cells (each cell is 8x8 pixels)
    constexpr int CELL_SIZE = 8;
    constexpr int NUM_CELLS_X = NORM_SIZE / CELL_SIZE; // 4
    constexpr int NUM_CELLS_Y = NORM_SIZE / CELL_SIZE; // 4
    constexpr int BINS_PER_CELL = 16;                  // 16 quantized bins

    std::vector<float> descriptor;
    descriptor.reserve(NUM_CELLS_X * NUM_CELLS_Y * BINS_PER_CELL);

    for (int cy = 0; cy < NUM_CELLS_Y; ++cy) {
        for (int cx = 0; cx < NUM_CELLS_X; ++cx) {
            std::vector<float> cellHist(BINS_PER_CELL, 0.0f);
            float cellSum = 0.0f;

            for (int y = cy * CELL_SIZE; y < (cy + 1) * CELL_SIZE; ++y) {
                for (int x = cx * CELL_SIZE; x < (cx + 1) * CELL_SIZE; ++x) {
                    uint8_t center = norm.getPixel(x, y);
                    uint8_t code = 0;

                    // 8-neighbor sampling
                    const int dx[8] = {-1, 0, 1, 1, 1, 0, -1, -1};
                    const int dy[8] = {-1, -1, -1, 0, 1, 1, 1, 0};

                    for (int n = 0; n < 8; ++n) {
                        uint8_t neighbor = norm.getPixel(x + dx[n], y + dy[n]);
                        if (neighbor >= center) {
                            code |= static_cast<uint8_t>(1 << n);
                        }
                    }

                    // Quantize code (0-255) into 16 bins
                    int bin = code / 16;
                    cellHist[static_cast<size_t>(bin)] += 1.0f;
                    cellSum += 1.0f;
                }
            }

            // Normalize cell histogram (L1 norm)
            if (cellSum > 0.0f) {
                for (float& b : cellHist) b /= cellSum;
            }
            descriptor.insert(descriptor.end(), cellHist.begin(), cellHist.end());
        }
    }

    return descriptor;
}

float FaceDescriptor::computeChiSquareDistance(const std::vector<float>& descA, const std::vector<float>& descB) {
    if (descA.size() != descB.size() || descA.empty()) return 1.0f;

    float dist = 0.0f;
    for (size_t i = 0; i < descA.size(); ++i) {
        float sum = descA[i] + descB[i];
        if (sum > 1e-6f) {
            float diff = descA[i] - descB[i];
            dist += (diff * diff) / sum;
        }
    }
    return 0.5f * dist;
}

float FaceDescriptor::computeEuclideanDistance(const std::vector<float>& descA, const std::vector<float>& descB) {
    if (descA.size() != descB.size() || descA.empty()) return 1.0f;

    float sum = 0.0f;
    for (size_t i = 0; i < descA.size(); ++i) {
        float diff = descA[i] - descB[i];
        sum += diff * diff;
    }
    return std::sqrt(sum);
}

} // namespace face_auth
