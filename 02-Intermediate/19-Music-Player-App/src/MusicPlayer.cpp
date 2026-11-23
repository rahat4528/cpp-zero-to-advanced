#include "MusicPlayer.hpp"
#include <algorithm>
#include <random>
#include <fstream>
#include <cstring>

namespace audio {

MusicPlayerEngine::MusicPlayerEngine() = default;

void MusicPlayerEngine::addTrack(const AudioMetadata& track) {
    m_playlist.push_back(track);
}

void MusicPlayerEngine::removeTrack(size_t index) {
    if (index < m_playlist.size()) {
        m_playlist.erase(m_playlist.begin() + index);
        if (m_currentIndex >= m_playlist.size() && !m_playlist.empty()) {
            m_currentIndex = m_playlist.size() - 1;
        }
    }
}

void MusicPlayerEngine::clearPlaylist() {
    stop();
    m_playlist.clear();
    m_currentIndex = 0;
}

bool MusicPlayerEngine::play() {
    if (m_playlist.empty()) return false;
    m_state = PlaybackState::Playing;
    return true;
}

bool MusicPlayerEngine::pause() {
    if (m_state == PlaybackState::Playing) {
        m_state = PlaybackState::Paused;
        return true;
    }
    return false;
}

bool MusicPlayerEngine::stop() {
    m_state = PlaybackState::Stopped;
    m_currentIndex = 0;
    return true;
}

bool MusicPlayerEngine::next() {
    if (m_playlist.empty()) return false;

    if (m_mode == PlaybackMode::RepeatOne) {
        // Stays on same track
        return true;
    } else if (m_mode == PlaybackMode::Shuffle) {
        static std::mt19937 rng(1337);
        std::uniform_int_distribution<size_t> dist(0, m_playlist.size() - 1);
        m_currentIndex = dist(rng);
        return true;
    } else if (m_currentIndex + 1 < m_playlist.size()) {
        m_currentIndex++;
        return true;
    } else if (m_mode == PlaybackMode::RepeatAll) {
        m_currentIndex = 0;
        return true;
    } else {
        stop();
        return false;
    }
}

bool MusicPlayerEngine::previous() {
    if (m_playlist.empty()) return false;

    if (m_currentIndex > 0) {
        m_currentIndex--;
        return true;
    } else if (m_mode == PlaybackMode::RepeatAll) {
        m_currentIndex = m_playlist.size() - 1;
        return true;
    }
    return false;
}

void MusicPlayerEngine::setVolume(float volume) {
    m_volume = std::clamp(volume, 0.0f, 1.0f);
}

float MusicPlayerEngine::getVolume() const {
    return m_volume;
}

void MusicPlayerEngine::setPlaybackMode(PlaybackMode mode) {
    m_mode = mode;
}

PlaybackMode MusicPlayerEngine::getPlaybackMode() const {
    return m_mode;
}

PlaybackState MusicPlayerEngine::getState() const {
    return m_state;
}

const AudioMetadata* MusicPlayerEngine::getCurrentTrack() const {
    if (m_playlist.empty() || m_currentIndex >= m_playlist.size()) {
        return nullptr;
    }
    return &m_playlist[m_currentIndex];
}

size_t MusicPlayerEngine::getCurrentTrackIndex() const {
    return m_currentIndex;
}

const std::vector<AudioMetadata>& MusicPlayerEngine::getPlaylist() const {
    return m_playlist;
}

#pragma pack(push, 1)
struct WavHeader {
    char riffTag[4];     // "RIFF"
    uint32_t riffSize;
    char waveTag[4];     // "WAVE"
    char fmtTag[4];      // "fmt "
    uint32_t fmtSize;
    uint16_t audioFormat;
    uint16_t numChannels;
    uint32_t sampleRate;
    uint32_t byteRate;
    uint16_t blockAlign;
    uint16_t bitsPerSample;
    char dataTag[4];     // "data"
    uint32_t dataSize;
};
#pragma pack(pop)

std::optional<AudioMetadata> MusicPlayerEngine::parseWavHeader(const std::string& wavPath) {
    std::ifstream in(wavPath, std::ios::binary);
    if (!in.is_open()) return std::nullopt;

    WavHeader hdr;
    in.read(reinterpret_cast<char*>(&hdr), sizeof(hdr));
    if (in.gcount() != sizeof(hdr)) return std::nullopt;

    if (std::memcmp(hdr.riffTag, "RIFF", 4) != 0 ||
        std::memcmp(hdr.waveTag, "WAVE", 4) != 0 ||
        std::memcmp(hdr.fmtTag, "fmt ", 4) != 0) {
        return std::nullopt;
    }

    AudioMetadata meta;
    meta.filePath = wavPath;
    meta.sampleRate = hdr.sampleRate;
    meta.channels = hdr.numChannels;
    meta.bitsPerSample = hdr.bitsPerSample;

    if (hdr.byteRate > 0) {
        meta.durationSeconds = static_cast<int>(hdr.dataSize / hdr.byteRate);
    }
    meta.title = "Audio Track";
    meta.artist = "Unknown Artist";
    meta.album = "Single";

    return meta;
}

} // namespace audio
