#include "ChatServer.hpp"
#include <iostream>
#include <string>
#include <chrono>

int main(int argc, char* argv[]) {
    int port = 8080;
    if (argc >= 2) {
        port = std::stoi(argv[1]);
    }

    std::cout << "=================================================================\n";
    std::cout << "              MULTI-THREADED TCP GROUP CHAT SERVER               \n";
    std::cout << "=================================================================\n";

    chat::ChatServer server(port);
    if (!server.start()) {
        std::cerr << "Failed to start server on port " << port << "\n";
        return 1;
    }

    if (argc >= 3 && std::string(argv[2]) == "--test-run") {
        std::cout << "[Test-Run Mode] Server initialized and listening. Running 1 second check...\n";
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        server.stop();
        std::cout << "[Test-Run Mode] Server shutdown clean.\n";
        return 0;
    }

    std::cout << "Server active. Press Enter to shutdown gracefully.\n";
    std::string dummy;
    std::getline(std::cin, dummy);

    server.stop();
    return 0;
}
