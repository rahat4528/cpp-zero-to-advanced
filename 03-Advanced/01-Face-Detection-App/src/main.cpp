#include "FaceDetector.hpp"
#include <iostream>
#include <iomanip>
#include <string>

int main(int argc, char* argv[]) {
    if (argc >= 2 && std::string(argv[1]) == "--test-run") {
        std::cout << "=================================================================\n";
        std::cout << "          COMPUTER VISION FACE DETECTION APP (HAAR CASCADE)      \n";
        std::cout << "=================================================================\n";
        std::cout << "[Test-Run Mode] Generating synthetic raster with facial features...\n";

        // Create 100x100 synthetic image
        vision::GrayscaleRaster img;
        img.width = 100;
        img.height = 100;
        img.pixels.resize(img.width * img.height, 180); // Base skin tone

        // Draw darker eye strip
        for (int y = 25; y < 35; ++y) {
            for (int x = 20; x < 60; ++x) {
                img.pixels[y * 100 + x] = 50; // Dark eyes
            }
        }
        // Draw bright nose bridge in center
        for (int y = 25; y < 45; ++y) {
            for (int x = 36; x < 44; ++x) {
                img.pixels[y * 100 + x] = 220; // Highlighted nose bridge
            }
        }

        vision::FaceDetectorCascade detector;
        auto detections = detector.detectFaces(img, 1.2, 1);

        std::cout << "Found " << detections.size() << " facial candidate region(s):\n";
        for (size_t i = 0; i < detections.size(); ++i) {
            const auto& d = detections[i];
            std::cout << "  Region [" << i + 1 << "]: (" << d.x << ", " << d.y << ") "
                      << d.width << "x" << d.height << " px | Confidence: "
                      << std::fixed << std::setprecision(2) << d.confidence << "\n";
        }

        std::cout << "[Test-Run Mode] Detection pipeline executed successfully.\n";
        return 0;
    }

    std::cout << "=================================================================\n";
    std::cout << "          COMPUTER VISION FACE DETECTION APP (HAAR CASCADE)      \n";
    std::cout << "=================================================================\n";
    std::cout << "Run with --test-run to execute automated vision pipeline.\n";
    return 0;
}
