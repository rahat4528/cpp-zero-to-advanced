#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>
#include "FaceDescriptor.hpp"
#include "FaceAuthManager.hpp"

using namespace face_auth;

static FaceImage makeFace(uint8_t skin, uint8_t eye, uint8_t mouth) {
    FaceImage img;
    img.width = 32;
    img.height = 32;
    img.grayscale.assign(1024, skin);
    for (int y = 8; y <= 12; ++y) {
        for (int x = 6; x <= 10; ++x) img.setPixel(x, y, eye);
        for (int x = 20; x <= 24; ++x) img.setPixel(x, y, eye);
    }
    for (int y = 22; y <= 26; ++y) {
        for (int x = 10; x <= 22; ++x) img.setPixel(x, y, mouth);
    }
    return img;
}

TEST_SUITE("Biometric Face Authentication System") {
    TEST_CASE("LBP Feature Extraction and Self-Distance") {
        auto face = makeFace(180, 30, 50);
        auto desc = FaceDescriptor::extractLBPFeatures(face);
        CHECK(desc.size() == 256); // 4x4 cells * 16 bins

        // Distance to identical self must be 0.0
        float selfDist = FaceDescriptor::computeChiSquareDistance(desc, desc);
        CHECK(selfDist == doctest::Approx(0.0f));
    }

    TEST_CASE("Genuine User Matching vs Imposter Rejection") {
        FaceAuthManager auth(0.15f);
        auto enrolledFace = makeFace(200, 20, 40);
        REQUIRE(auth.enrollUser("User1", enrolledFace));

        // Genuine attempt with slight noise
        auto genuineProbe = makeFace(198, 22, 42);
        auto genRes = auth.authenticate(genuineProbe);
        CHECK(genRes.authenticated);
        CHECK(genRes.matchedUser == "User1");
        CHECK(genRes.matchDistance < 0.15f);

        // Imposter attempt
        auto imposterFace = makeFace(60, 100, 150);
        auto impRes = auth.authenticate(imposterFace);
        CHECK_FALSE(impRes.authenticated);
        CHECK(impRes.matchDistance > 0.15f);
    }
}
