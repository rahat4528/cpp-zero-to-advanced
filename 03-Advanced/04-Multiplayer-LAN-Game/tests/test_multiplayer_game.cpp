#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>
#include "GameProtocol.hpp"
#include "AuthoritativeServer.hpp"
#include "GameClient.hpp"
#include <thread>
#include <chrono>

using namespace lan_game;

TEST_SUITE("Multiplayer LAN Game Engine") {
    TEST_CASE("WorldSnapshot serialization and round-trip parsing") {
        WorldSnapshot snap;
        snap.tick = 42;
        snap.players.push_back({1, "Alice", 120.5f, 240.75f, 100, 10});
        snap.players.push_back({2, "Bob", 300.0f, 150.0f, 85, 25});

        std::string serialized = snap.serialize();
        CHECK_FALSE(serialized.empty());

        auto parsed = WorldSnapshot::deserialize(serialized);
        REQUIRE(parsed.has_value());
        CHECK(parsed->tick == 42);
        REQUIRE(parsed->players.size() == 2);
        CHECK(parsed->players[0].name == "Alice");
        CHECK(parsed->players[0].x == doctest::Approx(120.5f));
        CHECK(parsed->players[1].name == "Bob");
        CHECK(parsed->players[1].health == 85);
    }

    TEST_CASE("Authoritative server movement validation and boundary clamping") {
        AuthoritativeServer server(0);
        int p1 = server.registerPlayer("Speedy");
        CHECK(server.getPlayerCount() == 1);

        // Apply input moving right
        server.applyPlayerInput(p1, {1, 10.0f, 0.0f});
        auto snap = server.getLatestSnapshot();
        REQUIRE(snap.players.size() == 1);
        float x1 = snap.players[0].x;
        CHECK(x1 > 0.0f);

        // Apply extreme input to check boundary clamp [0, 800]
        server.applyPlayerInput(p1, {2, 1000.0f, 0.0f});
        snap = server.getLatestSnapshot();
        CHECK(snap.players[0].x == doctest::Approx(800.0f));

        server.unregisterPlayer(p1);
        CHECK(server.getPlayerCount() == 0);
    }

    TEST_CASE("Network Client-Server handshake and snapshot exchange") {
        int testPort = 9245;
        AuthoritativeServer server(testPort);
        REQUIRE(server.start());

        std::this_thread::sleep_for(std::chrono::milliseconds(50));

        GameClient client;
        bool connected = client.connectToServer("127.0.0.1", testPort, "TestBot");
        REQUIRE(connected);
        CHECK(client.isConnected());
        CHECK(client.getLocalPlayerId() > 0);

        // Send control input
        CHECK(client.sendInput(1.0f, -0.5f));
        std::this_thread::sleep_for(std::chrono::milliseconds(150));

        auto snap = client.getLatestSnapshot();
        REQUIRE(snap.has_value());
        CHECK(snap->players.size() >= 1);

        client.disconnect();
        CHECK_FALSE(client.isConnected());
        server.stop();
    }
}
