#include "ChunkDownloader.hpp"
#include <fstream>
#include <chrono>
#include <thread>
#include <iostream>

#if defined(_WIN32) || defined(_WIN64)
    #ifndef WIN32_LEAN_AND_MEAN
    #define WIN32_LEAN_AND_MEAN
    #endif
    #include <windows.h>
    #include <wininet.h>
#endif

namespace media_downloader {

ChunkDownloader::ChunkDownloader() = default;

void ChunkDownloader::setProgressCallback(ProgressCallback cb) {
    m_progressCb = std::move(cb);
}

bool ChunkDownloader::simulateDownload(size_t totalBytes, const std::string& outputPath, size_t chunkSize) {
    std::ofstream out(outputPath, std::ios::binary);
    if (!out) return false;

    size_t downloaded = 0;
    std::vector<char> dummyChunk(chunkSize, 'V');
    auto startTime = std::chrono::steady_clock::now();

    while (downloaded < totalBytes) {
        size_t toWrite = std::min(chunkSize, totalBytes - downloaded);
        out.write(dummyChunk.data(), static_cast<std::streamsize>(toWrite));
        downloaded += toWrite;

        auto now = std::chrono::steady_clock::now();
        double elapsedSec = std::chrono::duration<double>(now - startTime).count();
        double speedKBs = elapsedSec > 0.001 ? (static_cast<double>(downloaded) / 1024.0) / elapsedSec : 0.0;

        if (m_progressCb) {
            DownloadProgress p;
            p.bytesDownloaded = downloaded;
            p.totalBytes = totalBytes;
            p.percentComplete = (static_cast<double>(downloaded) / static_cast<double>(totalBytes)) * 100.0;
            p.speedKBs = speedKBs;
            m_progressCb(p);
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(5));
    }
    return true;
}

std::vector<uint8_t> ChunkDownloader::downloadMemory(const std::string& url) {
    std::vector<uint8_t> result;

#if defined(_WIN32) || defined(_WIN64)
    HINTERNET hInternet = InternetOpenA("CPP-Media-Downloader/1.0", INTERNET_OPEN_TYPE_PRECONFIG, nullptr, nullptr, 0);
    if (!hInternet) return result;

    HINTERNET hUrl = InternetOpenUrlA(hInternet, url.c_str(), nullptr, 0,
                                     INTERNET_FLAG_RELOAD | INTERNET_FLAG_NO_CACHE_WRITE | INTERNET_FLAG_SECURE, 0);
    if (!hUrl) {
        InternetCloseHandle(hInternet);
        return result;
    }

    char buffer[4096];
    DWORD bytesRead = 0;
    while (InternetReadFile(hUrl, buffer, sizeof(buffer), &bytesRead) && bytesRead > 0) {
        result.insert(result.end(), buffer, buffer + bytesRead);
    }

    InternetCloseHandle(hUrl);
    InternetCloseHandle(hInternet);
#else
    (void)url;
#endif

    return result;
}

bool ChunkDownloader::downloadToFile(const std::string& url, const std::string& outputPath, size_t chunkSize) {
    (void)chunkSize;
    auto data = downloadMemory(url);
    if (data.empty()) return false;

    std::ofstream out(outputPath, std::ios::binary);
    if (!out) return false;
    out.write(reinterpret_cast<const char*>(data.data()), static_cast<std::streamsize>(data.size()));
    return true;
}

} // namespace media_downloader
