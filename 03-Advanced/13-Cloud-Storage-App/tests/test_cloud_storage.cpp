#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>
#include "CloudProtocol.hpp"
#include "CloudServer.hpp"
#include "CloudClient.hpp"
#include <thread>
#include <chrono>

using namespace cloud_storage;

TEST_SUITE("Cloud Storage Client-Server Engine") {
    TEST_CASE("Hex conversion and SHA-256 calculation") {
        std::vector<uint8_t> data = {0x00, 0x12, 0xAB, 0xFF};
        std::string hex = bytesToHex(data);
        CHECK(hex == "0012abff");

        auto back = hexToBytes(hex);
        CHECK(back == data);

        std::string hash = computeSha256(data);
        CHECK(hash.size() == 64);
    }

    TEST_CASE("Cloud Storage Auth, Chunked Multi-Part Upload, and Download") {
        int testPort = 8350;
        CloudServer server(testPort);
        server.registerUser("alice", "PasswordAlice123");
        REQUIRE(server.start());

        std::this_thread::sleep_for(std::chrono::milliseconds(50));

        CloudClient client;
        REQUIRE(client.connectToServer("127.0.0.1", testPort));

        // Wrong credentials fail
        CHECK_FALSE(client.login("alice", "WrongPass"));

        // Correct credentials succeed
        REQUIRE(client.login("alice", "PasswordAlice123"));
        CHECK_FALSE(client.getAuthToken().empty());

        // Construct 100-byte test file
        std::vector<uint8_t> payload(100);
        for (size_t i = 0; i < payload.size(); ++i) {
            payload[i] = static_cast<uint8_t>(i ^ 0x5A);
        }

        // Upload in 16-byte chunks
        CHECK(client.uploadFile("binary_blob.dat", payload, 16));
        CHECK(server.getStoredFileCount("alice") == 1);

        // Download and verify integrity
        auto downloaded = client.downloadFile("binary_blob.dat");
        REQUIRE(downloaded.has_value());
        CHECK(downloaded->size() == payload.size());
        CHECK(*downloaded == payload);

        // Download nonexistent file fails
        CHECK_FALSE(client.downloadFile("missing_file.bin").has_value());

        client.disconnect();
        server.stop();
    }
}
