#pragma once

#include <string>
#include <vector>
#include <cstdint>
#include <optional>

namespace audio {

enum class PlaybackState {
    Stopped,
    Playing,
    Paused
};

enum class PlaybackMode {
    Normal,
    RepeatOne,
    RepeatAll,
    Shuffle
};

struct AudioMetadata {
    std::string title;
    std::string artist;
    std::string album;
    int durationSeconds{0};
    uint32_t sampleRate{44100};
    uint16_t channels{2};
    uint16_t bitsPerSample{16};
    std::string filePath;
};

class MusicPlayerEngine {
public:
    MusicPlayerEngine();

    void addTrack(const AudioMetadata& track);
    void removeTrack(size_t index);
    void clearPlaylist();

    bool play();
    bool pause();
    bool stop();
    bool next();
    bool previous();

    void setVolume(float volume); // 0.0f to 1.0f
    [[nodiscard]] float getVolume() const;

    void setPlaybackMode(PlaybackMode mode);
    [[nodiscard]] PlaybackMode getPlaybackMode() const;
    [[nodiscard]] PlaybackState getState() const;

    [[nodiscard]] const AudioMetadata* getCurrentTrack() const;
    [[nodiscard]] size_t getCurrentTrackIndex() const;
    [[nodiscard]] const std::vector<AudioMetadata>& getPlaylist() const;

    // RIFF WAVE file header parser
    static std::optional<AudioMetadata> parseWavHeader(const std::string& wavPath);

private:
    std::vector<AudioMetadata> m_playlist;
    size_t m_currentIndex{0};
    PlaybackState m_state{PlaybackState::Stopped};
    PlaybackMode m_mode{PlaybackMode::Normal};
    float m_volume{0.8f};
};

} // namespace audio
