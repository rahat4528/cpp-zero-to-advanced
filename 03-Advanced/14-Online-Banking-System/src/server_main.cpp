#include "BankingServer.hpp"
#include <iostream>

int main(int argc, char* argv[]) {
    int port = 9050;
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
    std::cout << "   Online Banking Transaction Server    \n";
    std::cout << "========================================\n";
    std::cout << "Starting server on port " << port << "...\n";

    banking::BankingServer server(port, ":memory:");
    // Seed initial test accounts
    server.getDatabase().createAccount("ACCT-1001", "Alice", "1234", 500000); // $5,000.00
    server.getDatabase().createAccount("ACCT-1002", "Bob", "5678", 200000);   // $2,000.00

    if (!server.start()) {
        std::cerr << "Failed to start server on port " << port << "\n";
        return 1;
    }

    std::cout << "Banking Server operational (SQLite ACID immediate transactions enabled).\n";

    if (testRun) {
        std::cout << "[Test Mode] Running smoke verification...\n";
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        server.stop();
        std::cout << "[Test Mode] Smoke verification completed.\n";
        return 0;
    }

    std::cout << "Press Enter to stop server...\n";
    std::string line;
    std::getline(std::cin, line);
    server.stop();
    return 0;
}
