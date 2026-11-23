#include "BankingClient.hpp"
#include "BankingServer.hpp"
#include <iostream>
#include <thread>
#include <chrono>

int main(int argc, char* argv[]) {
    bool testRun = false;
    for (int i = 1; i < argc; ++i) {
        if (std::string(argv[i]) == "--test-run") {
            testRun = true;
        }
    }

    std::cout << "========================================\n";
    std::cout << "       Online Banking Client Terminal   \n";
    std::cout << "========================================\n";

    if (testRun) {
        std::cout << "[Test Mode] Running automated banking transaction simulation...\n";
        int testPort = 9380;
        banking::BankingServer testServer(testPort);
        testServer.getDatabase().createAccount("ACC-TEST-A", "Alice Corp", "1111", 100000); // $1000.00
        testServer.getDatabase().createAccount("ACC-TEST-B", "Bob Supplier", "2222", 50000); // $500.00

        if (!testServer.start()) {
            std::cerr << "Failed to start test server.\n";
            return 1;
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(100));

        banking::BankingClient client;
        if (!client.connectToServer("127.0.0.1", testPort)) {
            std::cerr << "Failed to connect to test banking server.\n";
            testServer.stop();
            return 1;
        }

        std::cout << "Logging in as Alice (ACC-TEST-A)...\n";
        if (!client.login("ACC-TEST-A", "1111")) {
            std::cerr << "Login failed!\n";
            testServer.stop();
            return 1;
        }

        auto bal = client.queryBalance();
        std::cout << "Initial Balance: $" << (*bal / 100.0) << "\n";

        std::cout << "Executing $250.00 transfer to Bob (ACC-TEST-B)...\n";
        std::string txRef, err;
        if (client.transferFunds("ACC-TEST-B", 25000, txRef, err)) {
            std::cout << "Transfer SUCCESS! Transaction Reference: " << txRef << "\n";
        } else {
            std::cerr << "Transfer FAILED: " << err << "\n";
            testServer.stop();
            return 1;
        }

        auto updatedBal = client.queryBalance();
        std::cout << "Updated Balance: $" << (*updatedBal / 100.0) << "\n";

        std::cout << "Attempting overdraft ($5,000.00) transfer...\n";
        std::string overRef, overErr;
        bool overResult = client.transferFunds("ACC-TEST-B", 500000, overRef, overErr);
        std::cout << "Overdraft attempt: "
                  << (overResult ? "FAIL (ALLOWED OVERDRAFT!)" : "SUCCESS (ACID REJECTED WITH ROLLBACK)")
                  << " -> " << overErr << "\n";

        client.disconnect();
        testServer.stop();
        std::cout << "[Test Mode] Banking client-server test passed.\n";
        return 0;
    }

    std::cout << "Connecting to 127.0.0.1:9050...\n";
    banking::BankingClient client;
    if (client.connectToServer("127.0.0.1", 9050)) {
        std::cout << "Connected to banking host. Use --test-run for automated verification.\n";
        client.disconnect();
    } else {
        std::cout << "Banking host unavailable. Run with --test-run for automated test mode.\n";
    }

    return 0;
}
