#include "CloudClient.hpp"
#include "CloudServer.hpp"
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
    std::cout << "     Cloud Storage Client Engine        \n";
    std::cout << "========================================\n";

    if (testRun) {
        std::cout << "[Test Mode] Running automated loopback client-server verification...\n";
        int testPort = 8192;
        cloud_storage::CloudServer testServer(testPort);
        if (!testServer.start()) {
            std::cerr << "Failed to start local test server.\n";
            return 1;
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(100));

        cloud_storage::CloudClient client;
        if (!client.connectToServer("127.0.0.1", testPort)) {
            std::cerr << "Failed to connect to local test server.\n";
            testServer.stop();
            return 1;
        }

        std::cout << "Authenticating as 'developer'...\n";
        if (!client.login("developer", "DevPass2026")) {
            std::cerr << "Authentication failed!\n";
            testServer.stop();
            return 1;
        }
        std::cout << "Session established with Token: " << client.getAuthToken() << "\n";

        std::string sampleData = "Sample Cloud Storage Object Payload - Block 0123456789";
        std::vector<uint8_t> uploadBytes(sampleData.begin(), sampleData.end());

        std::cout << "Uploading 'readme.txt' (" << uploadBytes.size() << " bytes) in 16-byte chunks...\n";
        if (!client.uploadFile("readme.txt", uploadBytes, 16)) {
            std::cerr << "Chunked upload failed!\n";
            testServer.stop();
            return 1;
        }
        std::cout << "Chunked upload and SHA-256 validation verified by server.\n";

        std::cout << "Downloading 'readme.txt'...\n";
        auto downloaded = client.downloadFile("readme.txt");
        if (downloaded && *downloaded == uploadBytes) {
            std::cout << "Download SUCCESS: Content matches byte-for-byte!\n";
        } else {
            std::cerr << "Download integrity verification failed!\n";
            testServer.stop();
            return 1;
        }

        client.disconnect();
        testServer.stop();
        std::cout << "[Test Mode] Cloud client-server test passed.\n";
        return 0;
    }

    std::cout << "Connecting to 127.0.0.1:8080...\n";
    cloud_storage::CloudClient client;
    if (client.connectToServer("127.0.0.1", 8080)) {
        if (client.login("admin", "AdminSecret123")) {
            std::cout << "Connected & Authenticated as admin. Token: " << client.getAuthToken() << "\n";
        }
        client.disconnect();
    } else {
        std::cout << "Could not connect to standalone server. Use --test-run for automated verification.\n";
    }

    return 0;
}
