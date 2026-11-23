#include "MockMediaServer.hpp"

namespace media_downloader {

MediaMetadata MockMediaServer::getMetadata(const std::string& videoId) {
    MediaMetadata meta;
    meta.videoId = videoId;
    meta.title = "Educational C++ Modern Architecture Lecture (" + videoId + ")";
    meta.contentSizeBytes = 512 * 1024; // 512 KB
    meta.mimeType = "video/mp4";
    return meta;
}

std::vector<uint8_t> MockMediaServer::generateVideoChunk(size_t offset, size_t length) {
    (void)offset;
    std::vector<uint8_t> chunk(length);
    for (size_t i = 0; i < length; ++i) {
        chunk[i] = static_cast<uint8_t>((i + 37) % 256);
    }
    return chunk;
}

} // namespace media_downloader
