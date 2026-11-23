#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>
#include "FaceDetector.hpp"

TEST_CASE("IntegralImage - O(1) Rectangle Sum Verification") {
    vision::GrayscaleRaster raster;
    raster.width = 4;
    raster.height = 4;
    raster.pixels = {
        1, 1, 1, 1,
        2, 2, 2, 2,
        3, 3, 3, 3,
        4, 4, 4, 4
    };

    vision::IntegralImage ii(raster);
    CHECK(ii.getWidth() == 4);
    CHECK(ii.getHeight() == 4);

    // Full 4x4 sum = (1*4) + (2*4) + (3*4) + (4*4) = 4 + 8 + 12 + 16 = 40
    CHECK(ii.getRectSum(0, 0, 4, 4) == 40);

    // Top-left 2x2 sum: [1, 1], [2, 2] = 6
    CHECK(ii.getRectSum(0, 0, 2, 2) == 6);

    // Bottom-right 2x2 sum: [3, 3], [4, 4] = 14
    CHECK(ii.getRectSum(2, 2, 2, 2) == 14);

    // Out of bounds check
    CHECK(ii.getRectSum(-1, 0, 2, 2) == 0);
}

TEST_CASE("DetectionRect - Intersection Over Union (IoU) Math") {
    vision::DetectionRect a{0, 0, 10, 10, 1.0};
    vision::DetectionRect b{0, 0, 10, 10, 0.9}; // Exactly identical
    CHECK(a.intersectionOverUnion(b) == doctest::Approx(1.0));

    vision::DetectionRect c{20, 20, 10, 10, 0.5}; // Disjoint
    CHECK(a.intersectionOverUnion(c) == doctest::Approx(0.0));

    vision::DetectionRect d{5, 0, 10, 10, 0.8}; // 50% horizontal overlap
    // Intersection = 5 * 10 = 50. Union = 100 + 100 - 50 = 150. IoU = 50/150 = 0.3333
    CHECK(a.intersectionOverUnion(d) == doctest::Approx(0.3333).epsilon(0.01));
}

TEST_CASE("FaceDetectorCascade - Non-Max Suppression Clustering") {
    std::vector<vision::DetectionRect> raw = {
        {10, 10, 30, 30, 50.0},
        {12, 11, 30, 30, 45.0}, // heavily overlapping duplicate
        {80, 80, 20, 20, 30.0}  // separate region
    };

    auto filtered = vision::FaceDetectorCascade::applyNonMaxSuppression(raw, 0.3);
    REQUIRE(filtered.size() == 2);
    CHECK(filtered[0].confidence == 50.0);
    CHECK(filtered[1].confidence == 30.0);
}
