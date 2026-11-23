#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest/doctest.h"
#include "SpeedTester.h"

using namespace speed_tester;

TEST_CASE("Bandwidth and unit conversions") {
    // 1,000,000 bytes = 8,000,000 bits = 8 Megabits
    CHECK(SpeedTester::bytesToMegabits(1000000) == doctest::Approx(8.0));

    // 1,048,576 bytes = 1 Megabyte
    CHECK(SpeedTester::bytesToMegabytes(1048576) == doctest::Approx(1.0));

    // 10,000,000 bytes transferred in 1.0 second = 80 Mbps
    double mbps = SpeedTester::calculateSpeedMbps(10000000, 1.0);
    CHECK(mbps == doctest::Approx(80.0));

    // 10,000,000 bytes transferred in 2.0 seconds = 40 Mbps
    double mbpsHalf = SpeedTester::calculateSpeedMbps(10000000, 2.0);
    CHECK(mbpsHalf == doctest::Approx(40.0));

    // Zero duration handling
    CHECK(SpeedTester::calculateSpeedMbps(1000, 0.0) == doctest::Approx(0.0));
}

TEST_CASE("Mock speed test execution") {
    SpeedTester tester;
    // Test with 5 MB mock payload
    auto res = tester.runTest("", true, 5000000);

    CHECK(res.success == true);
    CHECK(res.isMock == true);
    CHECK(res.bytesDownloaded == 5000000);
    CHECK(res.durationSeconds > 0.0);
    CHECK(res.speedMbps > 0.0);
    CHECK(res.speedMBps > 0.0);
    CHECK(res.latencyMs > 0.0);
}

TEST_CASE("Report string formatting") {
    SpeedTestResult res;
    res.success = true;
    res.isMock = true;
    res.latencyMs = 25.5;
    res.bytesDownloaded = 1048576;
    res.durationSeconds = 1.0;
    res.speedMbps = 8.3886;
    res.speedMBps = 1.0;

    std::string report = SpeedTester::formatReport(res);
    CHECK(report.find("SUCCESS") != std::string::npos);
    CHECK(report.find("25.50 ms") != std::string::npos);
    CHECK(report.find("8.39 Mbps") != std::string::npos);
}
