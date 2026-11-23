#include "GameClient.hpp"
#include "AuthoritativeServer.hpp"
#include <iostream>
#include <thread>
#include <chrono>

int main(int argc, char* argv[]) {
    std::string host = "127.0.0.1";
    int port = 9000;
    std::string name = "Pilot";
    bool testRun = false;

    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "--test-run") {
            testRun = true;
        } else if (arg == "--host" && i + 1 < argc) {
            host = argv[++i];
        } else if (arg == "--port" && i + 1 < argc) {
            port = std::stoi(argv[++i]);
        } else if (arg == "--name" && i + 1 < argc) {
            name = argv[++i];
        }
    }

    if (testRun) {
        std::cout << "=== LAN Game Client (Test Mode) ===\n";
        int testPort = 9876;
        lan_game::AuthoritativeServer testServer(testPort);
        if (!testServer.start()) {
            std::cerr << "Failed to start local test server.\n";
            return 1;
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(100));

        lan_game::GameClient client;
        if (!client.connectToServer("127.0.0.1", testPort, "TestRacer")) {
            std::cerr << "Client failed to connect to test server.\n";
            testServer.stop();
            return 1;
        }

        std::cout << "Client successfully joined as Player ID: " << client.getLocalPlayerId() << "\n";
        client.sendInput(1.0f, 0.0f);
        std::this_thread::sleep_for(std::chrono::milliseconds(200));

        auto snap = client.getLatestSnapshot();
        if (snap) {
            std::cout << "Received snapshot tick #" << snap->tick << " with " << snap->players.size() << " player(s).\n";
        }

        client.disconnect();
        testServer.stop();
        std::cout << "Test run passed!\n";
        return 0;
    }

    std::cout << "=== LAN Game Client ===\n";
    std::cout << "Connecting to " << host << ":" << port << " as " << name << "...\n";

    lan_game::GameClient client;
    if (!client.connectToServer(host, port, name)) {
        std::cerr << "Failed to connect to game server.\n";
        return 1;
    }

    std::cout << "Connected! Local Player ID: " << client.getLocalPlayerId() << "\n";
    std::cout << "Sending simulated control inputs (Move Right)...\n";
    for (int i = 0; i < 5; ++i) {
        client.sendInput(1.0f, 0.0f);
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        auto snap = client.getLatestSnapshot();
        if (snap) {
            std::cout << "Tick: " << snap->tick << " | Players in world: " << snap->players.size() << "\n";
            for (const auto& p : snap->players) {
                std::cout << "  - ID " << p.id << " (" << p.name << ") at (" << p.x << ", " << p.y << ") Score: " << p.score << "\n";
            }
        }
    }

    client.disconnect();
    std::cout << "Disconnected from server.\n";
    return 0;
}
