#include "OfflineChat.h"

#include <iostream>
#include <sstream>
#include <string>

void printHelp() {
    std::cout << "Offline Chat App\n"
              << "Usage:\n"
              << "  OfflineChat                             Interactive Bot chat\n"
              << "  OfflineChat --mode two                  Two-user alternating mode\n"
              << "  OfflineChat --scripted \"hello;time;bye\" Automated test conversation\n"
              << "  OfflineChat --log <path>                Auto-export transcript on exit\n"
              << "  OfflineChat --help                      Show help\n";
}

int main(int argc, char* argv[]) {
    offline_chat::ChatMode mode = offline_chat::ChatMode::Bot;
    std::string scriptedInput;
    std::string autoLogPath;

    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "--help" || arg == "-h") {
            printHelp();
            return 0;
        } else if (arg == "--mode" && i + 1 < argc) {
            std::string m = argv[++i];
            if (m == "two" || m == "twouser") {
                mode = offline_chat::ChatMode::TwoUser;
            }
        } else if (arg == "--scripted" && i + 1 < argc) {
            scriptedInput = argv[++i];
        } else if (arg == "--log" && i + 1 < argc) {
            autoLogPath = argv[++i];
        }
    }

    offline_chat::ChatEngine engine(mode);

    if (!scriptedInput.empty()) {
        std::stringstream ss(scriptedInput);
        std::string item;
        while (std::getline(ss, item, ';')) {
            std::cout << "User: " << item << "\n";
            if (engine.isCommand(item)) {
                std::cout << engine.executeCommand(item) << "\n";
            } else {
                engine.sendMessage("User", item);
                const auto& hist = engine.getHistory();
                if (!hist.empty() && hist.back().sender == "CyberBot") {
                    std::cout << "CyberBot: " << hist.back().content << "\n";
                }
            }
        }
        if (!autoLogPath.empty()) {
            engine.exportLog(autoLogPath);
            std::cout << "Exported transcript to " << autoLogPath << "\n";
        }
        return 0;
    }

    std::cout << "=== Modern C++ Offline Chat Simulation ===\n";
    std::cout << "Type /help for commands, or /exit to quit.\n";
    std::string currentUser = "User1";

    std::string input;
    while (true) {
        std::cout << currentUser << " > ";
        if (!std::getline(std::cin, input) || input == "/exit") {
            break;
        }
        if (input.empty()) continue;

        if (engine.isCommand(input)) {
            std::cout << engine.executeCommand(input) << "\n";
        } else {
            engine.sendMessage(currentUser, input);
            if (engine.getMode() == offline_chat::ChatMode::Bot) {
                const auto& hist = engine.getHistory();
                if (!hist.empty() && hist.back().sender == "CyberBot") {
                    std::cout << "CyberBot > " << hist.back().content << "\n";
                }
            } else {
                // Toggle alternating users
                currentUser = (currentUser == "User1") ? "User2" : "User1";
            }
        }
    }

    if (!autoLogPath.empty()) {
        engine.exportLog(autoLogPath);
        std::cout << "Saved chat transcript to " << autoLogPath << "\n";
    }

    std::cout << "Chat session finished.\n";
    return 0;
}
