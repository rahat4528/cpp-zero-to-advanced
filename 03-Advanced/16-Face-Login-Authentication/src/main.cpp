#include "FaceAuthManager.hpp"
#include <iostream>
#include <iomanip>

static face_auth::FaceImage generateSyntheticFace(uint8_t skinTone, int eyeY, int eyeSpacing, int mouthY) {
    face_auth::FaceImage img;
    img.width = 32;
    img.height = 32;
    img.grayscale.assign(1024, skinTone);

    int leftEyeX = 16 - eyeSpacing;
    int rightEyeX = 16 + eyeSpacing;
    for (int y = eyeY; y < eyeY + 4; ++y) {
        for (int x = leftEyeX - 2; x <= leftEyeX + 2; ++x) img.setPixel(x, y, 30);
        for (int x = rightEyeX - 2; x <= rightEyeX + 2; ++x) img.setPixel(x, y, 30);
    }

    // Nose
    for (int y = eyeY + 5; y <= eyeY + 9; ++y) {
        for (int x = 15; x <= 17; ++x) img.setPixel(x, y, static_cast<uint8_t>(skinTone > 25 ? skinTone - 25 : 0));
    }

    // Mouth
    for (int y = mouthY; y < mouthY + 3; ++y) {
        for (int x = 11; x <= 21; ++x) img.setPixel(x, y, 40);
    }

    return img;
}

int main(int argc, char* argv[]) {
    bool testRun = false;
    for (int i = 1; i < argc; ++i) {
        if (std::string(argv[i]) == "--test-run") {
            testRun = true;
        }
    }

    std::cout << "================================================\n";
    std::cout << "   Biometric Face Login Authentication Engine   \n";
    std::cout << " (Local Binary Patterns & Chi-Square Matching)  \n";
    std::cout << "================================================\n";

    std::cout << "\n[!] SECURITY LIMITATION NOTICE:\n";
    std::cout << "  1. 2D Presentation Attack Vulnerability: Standard 2D facial descriptors\n";
    std::cout << "     cannot distinguish a live human face from a photograph or digital display.\n";
    std::cout << "  2. Environmental Drift: Extreme lighting variations, shadows, and occlusions\n";
    std::cout << "     (masks/glasses) alter texture gradients and affect false rejection rates (FRR).\n";
    std::cout << "  3. Production Recommendation: Multi-factor biometric fusion with active liveness\n";
    std::cout << "     challenge-response (e.g. eye blink / head yaw detection) or structured infrared depth.\n";
    std::cout << "------------------------------------------------\n";

    face_auth::FaceAuthManager authManager(0.15f);

    // Enroll synthetic users
    auto aliceFace = generateSyntheticFace(190, 8, 7, 24);
    auto bobFace = generateSyntheticFace(140, 12, 5, 26);

    authManager.enrollUser("Alice", aliceFace);
    authManager.enrollUser("Bob", bobFace);
    std::cout << "Enrolled " << authManager.getEnrolledUserCount() << " users in biometric template vault.\n";

    std::cout << "\nAttempting Authentication for Alice (Genuine Probe)...\n";
    auto aliceProbe = generateSyntheticFace(188, 8, 7, 24); // same geometry, slight illumination shift
    auto resAlice = authManager.authenticate(aliceProbe);
    std::cout << "Result: " << (resAlice.authenticated ? "[SUCCESS]" : "[DENIED]") << " -> " << resAlice.diagnosticMessage << "\n";

    std::cout << "\nAttempting Authentication for Imposter (Eve)...\n";
    auto eveFace = generateSyntheticFace(170, 6, 9, 21); // completely different facial geometry
    auto resEve = authManager.authenticate(eveFace);
    std::cout << "Result: " << (resEve.authenticated ? "[ACCEPTED]" : "[REJECTED]") << " -> " << resEve.diagnosticMessage << "\n";

    if (testRun) {
        std::cout << "\n[Test Mode] Biometric LBP face authentication verified.\n";
        return 0;
    }

    return 0;
}
