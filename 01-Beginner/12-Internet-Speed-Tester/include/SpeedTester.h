#pragma once

#include <chrono>
#include <string>
#include <vector>

namespace speed_tester {

struct SpeedTestResult {
    double latencyMs{0.0};
    size_t bytesDownloaded{0};
    double durationSeconds{0.0};
    double speedMbps{0.0};
    double speedMBps{0.0};
    bool isMock{false};
    bool success{false};
    std::string errorMessage;
};

class SpeedTester {
public:
    SpeedTester() = default;

    // Measures HTTP download speed against target URL (or mock payload)
    SpeedTestResult runTest(const std::string& url, bool useMock = false, size_t mockByteCount = 5000000);

    // Static mathematical throughput calculation functions
    static double bytesToMegabits(size_t bytes);
    static double bytesToMegabytes(size_t bytes);
    static double calculateSpeedMbps(size_t bytes, double durationSeconds);
    static double calculateSpeedMBps(size_t bytes, double durationSeconds);

    // Formats a summary speed report
    static std::string formatReport(const SpeedTestResult& res);
};

} // namespace speed_tester
