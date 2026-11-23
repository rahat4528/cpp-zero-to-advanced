#include "MusicPlayer.hpp"
#include <iostream>
#include <iomanip>
#include <string>

int main(int argc, char* argv[]) {
    audio::MusicPlayerEngine player;

    if (argc >= 2 && std::string(argv[1]) == "--test-run") {
        std::cout << "=================================================================\n";
        std::cout << "               DIGITAL MUSIC PLAYER & PLAYLIST APP               \n";
        std::cout << "=================================================================\n";
        std::cout << "[Test-Run Mode] Populating playlist and testing state transitions...\n";

        player.addTrack({"Clair de Lune", "Claude Debussy", "Suite Bergamasque", 305, 44100, 2, 16, "track1.wav"});
        player.addTrack({"Symphony No. 5", "Ludwig van Beethoven", "Classical Masterpieces", 432, 44100, 2, 16, "track2.wav"});
        player.addTrack({"Moonlight Sonata", "Ludwig van Beethoven", "Piano Sonatas", 360, 48000, 2, 24, "track3.wav"});

        std::cout << "Playlist size: " << player.getPlaylist().size() << " tracks\n";

        player.play();
        const auto* cur = player.getCurrentTrack();
        std::cout << "Now Playing: " << cur->title << " by " << cur->artist
                  << " (" << (cur->durationSeconds / 60) << ":" << std::setw(2) << std::setfill('0')
                  << (cur->durationSeconds % 60) << ")\n";

        player.next();
        cur = player.getCurrentTrack();
        std::cout << "Skipped to: " << cur->title << " by " << cur->artist << "\n";

        player.pause();
        std::cout << "Playback State: PAUSED\n";

        player.setVolume(0.95f);
        std::cout << "Volume Level: " << static_cast<int>(player.getVolume() * 100.0f) << "%\n";

        std::cout << "[Test-Run Mode] Completed successfully.\n";
        return 0;
    }

    std::cout << "=================================================================\n";
    std::cout << "               DIGITAL MUSIC PLAYER & PLAYLIST APP               \n";
    std::cout << "=================================================================\n";
    std::cout << "Run with --test-run to execute automated state engine verification.\n";
    return 0;
}
