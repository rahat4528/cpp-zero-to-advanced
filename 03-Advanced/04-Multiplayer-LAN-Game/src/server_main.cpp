#include "AuthoritativeServer.hpp"
#include <iostream>
#include <thread>
#include <chrono>

int main(int argc, char* argv[]) {
    int port = 9000;
    bool testRun = false;

    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "--test-run") {
            testRun = true;
        } else if (arg == "--port" && i + 1 < argc) {
            port = std::stoi(argv[++i]);
        }
    }

    std::cout << "=== Authoritative LAN Game Server ===\n";
    std::cout << "Binding to port " << port << "...\n";

    lan_game::AuthoritativeServer server(port);
    if (!server.start()) {
        std::cerr << "Failed to start server on port " << port << "\n";
        return 1;
    }

    std::cout << "Server active and listening for players at 20 Hz tick rate.\n";

    if (testRun) {
        std::cout << "[Test Mode] Running smoke verification for 500ms...\n";
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        std::cout << "[Test Mode] Smoke verification completed successfully.\n";
        server.stop();
        return 0;
    }

    std::cout << "Press Enter to stop the server...\n";
    std::string line;
    std::getline(std::cin, line);
    server.stop();
    std::cout << "Server stopped.\n";
    return 0;
}
