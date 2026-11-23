#pragma once

#include <string>
#include <vector>
#include <functional>
#include <cstdint>

namespace media_downloader {

struct DownloadProgress {
    size_t bytesDownloaded{0};
    size_t totalBytes{0};
    double percentComplete{0.0};
    double speedKBs{0.0};
};

using ProgressCallback = std::function<void(const DownloadProgress&)>;

class ChunkDownloader {
public:
    ChunkDownloader();
    ~ChunkDownloader() = default;

    void setProgressCallback(ProgressCallback cb);
    [[nodiscard]] bool downloadToFile(const std::string& url, const std::string& outputPath, size_t chunkSize = 65536);
    [[nodiscard]] std::vector<uint8_t> downloadMemory(const std::string& url);

    // Deterministic simulation for headless/offline testing
    [[nodiscard]] bool simulateDownload(size_t totalBytes, const std::string& outputPath, size_t chunkSize = 32768);

private:
    ProgressCallback m_progressCb;
};

} // namespace media_downloader
