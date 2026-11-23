#pragma once

#include <string>
#include <vector>
#include <cstdint>

namespace media_downloader {

struct MediaMetadata {
    std::string videoId;
    std::string title;
    size_t contentSizeBytes{0};
    std::string mimeType;
};

class MockMediaServer {
public:
    [[nodiscard]] static MediaMetadata getMetadata(const std::string& videoId);
    [[nodiscard]] static std::vector<uint8_t> generateVideoChunk(size_t offset, size_t length);
};

} // namespace media_downloader
