#include "VoiceCommandEngine.hpp"
#include <iostream>
#include <iomanip>
#include <string>

int main(int argc, char* argv[]) {
    voice::VoiceCommandEngine engine;

    // Register handlers
    engine.setActionHandler(voice::VoiceAction::VolumeUp, [](const voice::CommandMatch& m) {
        std::cout << "  -> [ACTION EXECUTED] Audio level boosted +10% (Confidence: "
                  << std::fixed << std::setprecision(1) << (m.confidence * 100.0) << "%)\n";
    });
    engine.setActionHandler(voice::VoiceAction::SystemStatus, [](const voice::CommandMatch& m) {
        std::cout << "  -> [ACTION EXECUTED] System CPU: 12% | RAM: 4.2 GB / 16 GB | Status: Optimal (Conf: "
                  << std::fixed << std::setprecision(1) << (m.confidence * 100.0) << "%)\n";
    });
    engine.setActionHandler(voice::VoiceAction::MuteAudio, [](const voice::CommandMatch& m) {
        std::cout << "  -> [ACTION EXECUTED] Audio muted (Conf: "
                  << std::fixed << std::setprecision(1) << (m.confidence * 100.0) << "%)\n";
    });

    if (argc >= 2 && std::string(argv[1]) == "--test-run") {
        std::cout << "=================================================================\n";
        std::cout << "             OFFLINE VOICE COMMAND DISPATCH ENGINE               \n";
        std::cout << "=================================================================\n";
        std::cout << "[Test-Run Mode] Feeding simulated acoustic voice utterances...\n\n";

        std::vector<std::string> simulatedVoiceInputs = {
            "volume up",                    // Exact match
            "volum up",                     // Acoustic typo/slur (fuzzy match)
            "system status",                // Exact match
            "mute",                         // Exact match
            "play something completely unk" // Unmatched
        };

        for (const auto& input : simulatedVoiceInputs) {
            std::cout << "Heard utterance: \"" << input << "\"\n";
            auto match = engine.processUtterance(input);
            if (match.isRecognized) {
                std::cout << "  Matched Intent: " << match.intent << "\n";
                engine.dispatch(match);
            } else {
                std::cout << "  [!] Utterance not recognized as command.\n";
            }
            std::cout << "\n";
        }

        std::cout << "[Test-Run Mode] Completed successfully.\n";
        return 0;
    }

    std::cout << "=================================================================\n";
    std::cout << "             OFFLINE VOICE COMMAND DISPATCH ENGINE               \n";
    std::cout << "=================================================================\n";
    std::cout << "Run with --test-run to execute automated acoustic simulation.\n";
    return 0;
}
