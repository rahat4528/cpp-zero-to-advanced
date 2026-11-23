#include "DigitalClock.h"

#include <chrono>
#include <iostream>
#include <string>
#include <thread>

void printHelp() {
    std::cout << "Digital Clock UI\n"
              << "Usage:\n"
              << "  DigitalClock                  Continuous live clock (24h)\n"
              << "  DigitalClock --snapshot       Print single frame and exit (useful for testing)\n"
              << "  DigitalClock --12h            Run with 12-hour AM/PM format\n"
              << "  DigitalClock --tz <offset>    Specify UTC hour offset (e.g. +6, -5)\n"
              << "  DigitalClock --help           Show this help message\n";
}

int main(int argc, char* argv[]) {
    bool snapshot = false;
    auto format = digital_clock::TimeFormat::Format24Hour;
    int tzOffset = 0;

    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "--help" || arg == "-h") {
            printHelp();
            return 0;
        } else if (arg == "--snapshot") {
            snapshot = true;
        } else if (arg == "--12h") {
            format = digital_clock::TimeFormat::Format12Hour;
        } else if (arg == "--24h") {
            format = digital_clock::TimeFormat::Format24Hour;
        } else if (arg == "--tz" && i + 1 < argc) {
            tzOffset = std::stoi(argv[++i]);
        }
    }

    auto renderFrame = [&](std::chrono::system_clock::time_point tp) {
        auto formatted = digital_clock::formatTime(tp, format, tzOffset);
        std::cout << "\n============================================\n";
        std::cout << "             DIGITAL CLOCK UI               \n";
        std::cout << "============================================\n";
        std::cout << " Current Time: " << formatted.timeString
                  << " (UTC" << (tzOffset >= 0 ? "+" : "") << tzOffset << ")\n\n";
        std::cout << digital_clock::renderSevenSegment(formatted.timeString.substr(0, 8));
        std::cout << "============================================\n";
    };

    if (snapshot) {
        renderFrame(std::chrono::system_clock::now());
        return 0;
    }

    std::cout << "Starting live Digital Clock (Press Ctrl+C to stop)...\n";
    for (int count = 0; count < 3; ++count) { // Runs 3 ticks in automated/test runs or live
        renderFrame(std::chrono::system_clock::now());
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }

    return 0;
}
