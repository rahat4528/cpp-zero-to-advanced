#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>
#include "ChunkDownloader.hpp"
#include "MockMediaServer.hpp"
#include <filesystem>

using namespace media_downloader;

TEST_SUITE("Media Stream Chunk Downloader") {
    TEST_CASE("Mock Media Metadata Extraction") {
        auto meta = MockMediaServer::getMetadata("demo123");
        CHECK(meta.videoId == "demo123");
        CHECK(meta.contentSizeBytes > 0);
        CHECK(meta.mimeType == "video/mp4");
    }

    TEST_CASE("Simulated Chunk Download and Progress Notification") {
        ChunkDownloader downloader;
        int callbackCount = 0;
        size_t lastReportedBytes = 0;

        downloader.setProgressCallback([&](const DownloadProgress& p) {
            callbackCount++;
            lastReportedBytes = p.bytesDownloaded;
        });

        std::string testFile = "test_stream_output.bin";
        size_t testSize = 64 * 1024; // 64 KB

        bool res = downloader.simulateDownload(testSize, testFile, 16 * 1024);
        REQUIRE(res);
        CHECK(callbackCount > 0);
        CHECK(lastReportedBytes == testSize);
        CHECK(std::filesystem::exists(testFile));
        CHECK(std::filesystem::file_size(testFile) == testSize);

        std::filesystem::remove(testFile);
    }
}
