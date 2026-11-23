#pragma once

#include <vector>
#include <string>
#include <cstdint>

namespace face_auth {

struct FaceImage {
    int width{0};
    int height{0};
    std::vector<uint8_t> grayscale;

    [[nodiscard]] uint8_t getPixel(int x, int y) const {
        if (x < 0 || x >= width || y < 0 || y >= height) return 0;
        return grayscale[static_cast<size_t>(y * width + x)];
    }

    void setPixel(int x, int y, uint8_t val) {
        if (x >= 0 && x < width && y >= 0 && y < height) {
            grayscale[static_cast<size_t>(y * width + x)] = val;
        }
    }
};

class FaceDescriptor {
public:
    static constexpr int NORM_SIZE = 32;

    [[nodiscard]] static FaceImage resizeBilinear(const FaceImage& src, int targetW, int targetH);
    [[nodiscard]] static std::vector<float> extractLBPFeatures(const FaceImage& face);
    [[nodiscard]] static float computeChiSquareDistance(const std::vector<float>& descA, const std::vector<float>& descB);
    [[nodiscard]] static float computeEuclideanDistance(const std::vector<float>& descA, const std::vector<float>& descB);
};

} // namespace face_auth
