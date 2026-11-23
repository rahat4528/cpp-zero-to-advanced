#include "CloudServer.hpp"
#include <iostream>

int main(int argc, char* argv[]) {
    int port = 8080;
    bool testRun = false;

    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "--test-run") {
            testRun = true;
        } else if (arg == "--port" && i + 1 < argc) {
            port = std::stoi(argv[++i]);
        }
    }

    std::cout << "========================================\n";
    std::cout << "   Cloud Storage Multi-Tenant Server    \n";
    std::cout << "========================================\n";
    std::cout << "Starting server on port " << port << "...\n";

    cloud_storage::CloudServer server(port);
    if (!server.start()) {
        std::cerr << "Failed to bind and listen on port " << port << "\n";
        return 1;
    }

    std::cout << "Server active and awaiting authenticated client sessions.\n";

    if (testRun) {
        std::cout << "[Test Mode] Smoke verification running...\n";
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        server.stop();
        std::cout << "[Test Mode] Server smoke test completed.\n";
        return 0;
    }

    std::cout << "Press Enter to stop server...\n";
    std::string line;
    std::getline(std::cin, line);
    server.stop();
    return 0;
}
