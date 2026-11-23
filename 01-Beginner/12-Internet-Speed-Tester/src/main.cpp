#include "SpeedTester.h"

#include <iostream>
#include <string>

void printHelp() {
    std::cout << "Internet Speed Tester CLI\n"
              << "Usage:\n"
              << "  SpeedTester                        Live download test against default fast mirror\n"
              << "  SpeedTester --mock                 Run offline simulated test (no internet required)\n"
              << "  SpeedTester --url <http_url>       Custom target download URL\n"
              << "  SpeedTester --bytes <size>         Set mock download payload size in bytes\n"
              << "  SpeedTester --help                 Show help\n";
}

int main(int argc, char* argv[]) {
    bool mock = false;
    std::string testUrl = "http://speedtest.tele2.net/1MB.zip";
    size_t mockBytes = 10000000; // 10MB default mock

    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "--help" || arg == "-h") {
            printHelp();
            return 0;
        } else if (arg == "--mock") {
            mock = true;
        } else if (arg == "--url" && i + 1 < argc) {
            testUrl = argv[++i];
        } else if (arg == "--bytes" && i + 1 < argc) {
            mockBytes = std::stoull(argv[++i]);
        }
    }

    std::cout << "Starting Speed Test (" << (mock ? "Mock Mode" : testUrl) << ")...\n";
    speed_tester::SpeedTester tester;
    auto result = tester.runTest(testUrl, mock, mockBytes);
    std::cout << speed_tester::SpeedTester::formatReport(result);

    return result.success ? 0 : 1;
}
