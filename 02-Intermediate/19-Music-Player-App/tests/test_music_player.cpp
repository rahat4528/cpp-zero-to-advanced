#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>
#include "MusicPlayer.hpp"
#include <fstream>
#include <filesystem>

TEST_CASE("MusicPlayerEngine - Playlist State Machine") {
    audio::MusicPlayerEngine player;

    CHECK(player.getState() == audio::PlaybackState::Stopped);
    CHECK(player.getPlaylist().empty());

    player.addTrack({"Track 1", "Artist A", "Album 1", 180, 44100, 2, 16, "t1.wav"});
    player.addTrack({"Track 2", "Artist B", "Album 2", 240, 44100, 2, 16, "t2.wav"});
    player.addTrack({"Track 3", "Artist C", "Album 3", 200, 44100, 2, 16, "t3.wav"});

    CHECK(player.getPlaylist().size() == 3);

    SUBCASE("Play, Pause, Resume, Stop") {
        CHECK(player.play() == true);
        CHECK(player.getState() == audio::PlaybackState::Playing);
        CHECK(player.getCurrentTrack()->title == "Track 1");

        CHECK(player.pause() == true);
        CHECK(player.getState() == audio::PlaybackState::Paused);

        CHECK(player.play() == true);
        CHECK(player.getState() == audio::PlaybackState::Playing);

        CHECK(player.stop() == true);
        CHECK(player.getState() == audio::PlaybackState::Stopped);
    }

    SUBCASE("Next, Previous, and Repeat Modes") {
        player.play();
        CHECK(player.next() == true);
        CHECK(player.getCurrentTrack()->title == "Track 2");

        CHECK(player.next() == true);
        CHECK(player.getCurrentTrack()->title == "Track 3");

        // End of playlist in Normal mode stops playback
        CHECK(player.next() == false);
        CHECK(player.getState() == audio::PlaybackState::Stopped);

        // RepeatAll mode wraps around
        player.setPlaybackMode(audio::PlaybackMode::RepeatAll);
        player.play();
        player.next(); // to 1
        player.next(); // to 2
        CHECK(player.next() == true); // wraps to 0
        CHECK(player.getCurrentTrackIndex() == 0);
    }

    SUBCASE("Volume Clamping") {
        player.setVolume(1.5f);
        CHECK(player.getVolume() == 1.0f);

        player.setVolume(-0.5f);
        CHECK(player.getVolume() == 0.0f);
    }
}

TEST_CASE("MusicPlayerEngine - WAV Header Deserialization") {
    // Generate valid 44-byte synthetic PCM WAV header
    std::string wavPath = "temp_synthetic_test.wav";
    std::ofstream out(wavPath, std::ios::binary);

    struct {
        char riff[4] = {'R', 'I', 'F', 'F'};
        uint32_t size = 36 + 176400; // 1 second of 44.1kHz 16-bit stereo
        char wave[4] = {'W', 'A', 'V', 'E'};
        char fmt[4] = {'f', 'm', 't', ' '};
        uint32_t fmtSize = 16;
        uint16_t format = 1; // PCM
        uint16_t channels = 2;
        uint32_t sampleRate = 44100;
        uint32_t byteRate = 176400;
        uint16_t blockAlign = 4;
        uint16_t bitsPerSample = 16;
        char data[4] = {'d', 'a', 't', 'a'};
        uint32_t dataSize = 176400;
    } hdr;

    out.write(reinterpret_cast<const char*>(&hdr), sizeof(hdr));
    out.close();

    auto meta = audio::MusicPlayerEngine::parseWavHeader(wavPath);
    REQUIRE(meta.has_value());
    CHECK(meta->sampleRate == 44100);
    CHECK(meta->channels == 2);
    CHECK(meta->bitsPerSample == 16);
    CHECK(meta->durationSeconds == 1);

    std::filesystem::remove(wavPath);
}
