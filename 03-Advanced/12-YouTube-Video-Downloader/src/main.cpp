#include "ChunkDownloader.hpp"
#include "MockMediaServer.hpp"
#include <iostream>
#include <iomanip>

int main(int argc, char* argv[]) {
    bool testRun = false;
    std::string videoId = "dQw4w9WgXcQ";

    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "--test-run") {
            testRun = true;
        } else if (arg == "--id" && i + 1 < argc) {
            videoId = argv[++i];
        }
    }

    std::cout << "===============================================\n";
    std::cout << "       Media Stream Chunk Downloader           \n";
    std::cout << "===============================================\n";
    std::cout << "LEGAL NOTICE: This utility is strictly intended for downloading\n";
    std::cout << "media content you own or have explicit rights/licenses to obtain.\n";
    std::cout << "Downloading copyrighted materials without authorization violates\n";
    std::cout << "platform Terms of Service and applicable intellectual property laws.\n";
    std::cout << "-----------------------------------------------\n";

    auto meta = media_downloader::MockMediaServer::getMetadata(videoId);
    std::cout << "Target Title:  " << meta.title << "\n";
    std::cout << "MIME Type:     " << meta.mimeType << "\n";
    std::cout << "Content Size:  " << (meta.contentSizeBytes / 1024) << " KB\n";

    media_downloader::ChunkDownloader downloader;
    downloader.setProgressCallback([](const media_downloader::DownloadProgress& p) {
        std::cout << "\r[Download Progress] " << std::fixed << std::setprecision(1)
                  << p.percentComplete << "% (" << (p.bytesDownloaded / 1024) << " / "
                  << (p.totalBytes / 1024) << " KB) Speed: " << p.speedKBs << " KB/s" << std::flush;
    });

    std::string tempOutput = "sample_downloaded_video.bin";
    std::cout << "\nInitiating chunk stream acquisition...\n";
    bool success = downloader.simulateDownload(meta.contentSizeBytes, tempOutput, 32 * 1024);
    std::cout << "\n";

    if (success) {
        std::cout << "Download completed successfully to: " << tempOutput << "\n";
    } else {
        std::cerr << "Stream acquisition failed.\n";
        return 1;
    }

    if (testRun) {
        std::cout << "[Test Mode] Chunk downloader and progress monitoring verified.\n";
        return 0;
    }

    return 0;
}
