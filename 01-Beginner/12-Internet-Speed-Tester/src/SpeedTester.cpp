#include "SpeedTester.h"

#include <cmath>
#include <iomanip>
#include <sstream>
#include <thread>

#if defined(_WIN32)
#include <windows.h>
#include <wininet.h>
#if defined(_MSC_VER)
#pragma comment(lib, "wininet.lib")
#endif
#endif

namespace speed_tester {

double SpeedTester::bytesToMegabits(size_t bytes) {
    return (static_cast<double>(bytes) * 8.0) / 1000000.0;
}

double SpeedTester::bytesToMegabytes(size_t bytes) {
    return static_cast<double>(bytes) / (1024.0 * 1024.0);
}

double SpeedTester::calculateSpeedMbps(size_t bytes, double durationSeconds) {
    if (durationSeconds <= 0.0) return 0.0;
    return bytesToMegabits(bytes) / durationSeconds;
}

double SpeedTester::calculateSpeedMBps(size_t bytes, double durationSeconds) {
    if (durationSeconds <= 0.0) return 0.0;
    return bytesToMegabytes(bytes) / durationSeconds;
}

SpeedTestResult SpeedTester::runTest(const std::string& url, bool useMock, size_t mockByteCount) {
    SpeedTestResult result;

    if (useMock || url.empty()) {
        result.isMock = true;
        // Simulate network latency (25 ms) and simulated chunked throughput
        auto startLatency = std::chrono::steady_clock::now();
        std::this_thread::sleep_for(std::chrono::milliseconds(25));
        auto endLatency = std::chrono::steady_clock::now();
        result.latencyMs = std::chrono::duration<double, std::milli>(endLatency - startLatency).count();

        auto startDownload = std::chrono::steady_clock::now();
        // Simulate downloading chunks: 5MB in ~100ms (approx 400 Mbps)
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        auto endDownload = std::chrono::steady_clock::now();

        result.durationSeconds = std::chrono::duration<double>(endDownload - startDownload).count();
        result.bytesDownloaded = mockByteCount;
        result.speedMbps = calculateSpeedMbps(result.bytesDownloaded, result.durationSeconds);
        result.speedMBps = calculateSpeedMBps(result.bytesDownloaded, result.durationSeconds);
        result.success = true;
        return result;
    }

#if defined(_WIN32)
    // Perform real HTTP GET using WinINet (native cross-platform compatible C++ API on Windows)
    HINTERNET hInternet = InternetOpenA("SpeedTester/1.0", INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, 0);
    if (!hInternet) {
        result.success = false;
        result.errorMessage = "Failed to initialize network interface.";
        return result;
    }

    auto startLatency = std::chrono::steady_clock::now();
    HINTERNET hUrl = InternetOpenUrlA(hInternet, url.c_str(), NULL, 0,
                                     INTERNET_FLAG_RELOAD | INTERNET_FLAG_NO_CACHE_WRITE, 0);
    auto endLatency = std::chrono::steady_clock::now();
    result.latencyMs = std::chrono::duration<double, std::milli>(endLatency - startLatency).count();

    if (!hUrl) {
        InternetCloseHandle(hInternet);
        result.success = false;
        result.errorMessage = "Unable to connect to target URL: " + url;
        return result;
    }

    char buffer[16384];
    DWORD bytesRead = 0;
    size_t totalBytes = 0;

    auto startDownload = std::chrono::steady_clock::now();
    while (InternetReadFile(hUrl, buffer, sizeof(buffer), &bytesRead) && bytesRead > 0) {
        totalBytes += bytesRead;
        // Limit max download to 20MB for testing safety
        if (totalBytes > 20 * 1024 * 1024) break;
    }
    auto endDownload = std::chrono::steady_clock::now();

    InternetCloseHandle(hUrl);
    InternetCloseHandle(hInternet);

    result.durationSeconds = std::chrono::duration<double>(endDownload - startDownload).count();
    result.bytesDownloaded = totalBytes;
    result.speedMbps = calculateSpeedMbps(result.bytesDownloaded, result.durationSeconds);
    result.speedMBps = calculateSpeedMBps(result.bytesDownloaded, result.durationSeconds);
    result.success = (totalBytes > 0);
    if (!result.success) {
        result.errorMessage = "No data received from target host.";
    }
    return result;
#else
    // Fallback on non-Windows platforms if curl is not linked directly
    result.isMock = true;
    result.success = true;
    result.bytesDownloaded = mockByteCount;
    result.durationSeconds = 0.1;
    result.speedMbps = calculateSpeedMbps(result.bytesDownloaded, result.durationSeconds);
    result.speedMBps = calculateSpeedMBps(result.bytesDownloaded, result.durationSeconds);
    return result;
#endif
}

std::string SpeedTester::formatReport(const SpeedTestResult& res) {
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(2);
    oss << "========================================\n"
        << "       INTERNET SPEED TEST REPORT       \n"
        << "========================================\n"
        << "Status         : " << (res.success ? "SUCCESS" : "FAILED") << "\n"
        << "Test Mode      : " << (res.isMock ? "Mock / Simulated" : "Live Network") << "\n"
        << "Ping / Latency : " << res.latencyMs << " ms\n"
        << "Data Ingested  : " << bytesToMegabytes(res.bytesDownloaded) << " MB (" << res.bytesDownloaded << " bytes)\n"
        << "Test Duration  : " << res.durationSeconds << " s\n"
        << "----------------------------------------\n"
        << "Download Speed : " << res.speedMbps << " Mbps\n"
        << "Throughput     : " << res.speedMBps << " MB/s\n"
        << "========================================\n";
    if (!res.success && !res.errorMessage.empty()) {
        oss << "Error Details: " << res.errorMessage << "\n";
    }
    return oss.str();
}

} // namespace speed_tester
