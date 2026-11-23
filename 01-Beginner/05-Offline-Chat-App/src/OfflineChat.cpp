#include "OfflineChat.h"

#include <algorithm>
#include <cctype>
#include <ctime>
#include <fstream>
#include <iomanip>
#include <sstream>

namespace offline_chat {

static std::string toLower(std::string s) {
    std::transform(s.begin(), s.end(), s.begin(), [](unsigned char c) {
        return static_cast<char>(std::tolower(c));
    });
    return s;
}

ChatEngine::ChatEngine(ChatMode mode) : m_mode(mode) {}

std::string ChatEngine::getCurrentTimestamp() {
    auto now = std::chrono::system_clock::now();
    std::time_t tt = std::chrono::system_clock::to_time_t(now);
    std::tm timeInfo{};
#if defined(_WIN32)
    localtime_s(&timeInfo, &tt);
#else
    localtime_r(&tt, &timeInfo);
#endif
    std::ostringstream oss;
    oss << std::setfill('0')
        << std::setw(2) << timeInfo.tm_hour << ":"
        << std::setw(2) << timeInfo.tm_min << ":"
        << std::setw(2) << timeInfo.tm_sec;
    return oss.str();
}

std::string ChatEngine::generateBotResponse(const std::string& userPrompt) const {
    std::string lower = toLower(userPrompt);

    if (lower.find("hello") != std::string::npos || lower.find("hi") != std::string::npos || lower.find("hey") != std::string::npos) {
        return "Hello there! How can I assist you with modern C++ today?";
    }
    if (lower.find("how are you") != std::string::npos) {
        return "I am functioning within optimal algorithmic parameters. Ready for instructions.";
    }
    if (lower.find("cpp") != std::string::npos || lower.find("c++") != std::string::npos) {
        return "Modern C++ (C++17/20) provides RAII, type safety, and zero-cost abstractions.";
    }
    if (lower.find("time") != std::string::npos) {
        return "The local simulation time is " + getCurrentTimestamp() + ".";
    }
    if (lower.find("name") != std::string::npos) {
        return "I am CyberBot, a deterministic offline chatbot simulation engine.";
    }
    if (lower.find("bye") != std::string::npos || lower.find("exit") != std::string::npos) {
        return "Farewell! Have a productive coding session.";
    }

    return "Fascinating query! Could you elaborate further or type /help for commands?";
}

ChatMessage ChatEngine::sendMessage(const std::string& sender, const std::string& text) {
    ChatMessage msg{sender, text, getCurrentTimestamp()};
    m_history.push_back(msg);

    // If bot mode and sender is user, generate automated response
    if (m_mode == ChatMode::Bot && sender != "CyberBot") {
        std::string reply = generateBotResponse(text);
        ChatMessage botMsg{"CyberBot", reply, getCurrentTimestamp()};
        m_history.push_back(botMsg);
    }

    return msg;
}

bool ChatEngine::isCommand(const std::string& text) const {
    return !text.empty() && text.front() == '/';
}

std::string ChatEngine::executeCommand(const std::string& commandLine) {
    std::istringstream iss(commandLine);
    std::string cmd;
    iss >> cmd;
    cmd = toLower(cmd);

    if (cmd == "/help") {
        return "Available commands:\n"
               "  /help             Display this help message\n"
               "  /history          Print entire chat log\n"
               "  /clear            Clear session history\n"
               "  /mode <bot|two>   Toggle chat mode\n"
               "  /export <file>    Export chat transcript to disk\n"
               "  /exit             End chat session";
    }
    if (cmd == "/history") {
        std::ostringstream oss;
        for (const auto& msg : m_history) {
            oss << "[" << msg.timestamp << "] " << msg.sender << ": " << msg.content << "\n";
        }
        return oss.str().empty() ? "History is empty." : oss.str();
    }
    if (cmd == "/clear") {
        clearHistory();
        return "Session history cleared.";
    }
    if (cmd == "/mode") {
        std::string modeArg;
        iss >> modeArg;
        if (toLower(modeArg) == "two" || toLower(modeArg) == "twouser") {
            setMode(ChatMode::TwoUser);
            return "Switched mode to Two-User local conversation.";
        } else {
            setMode(ChatMode::Bot);
            return "Switched mode to Offline Bot.";
        }
    }
    if (cmd == "/export") {
        std::string path;
        iss >> path;
        if (path.empty()) path = "chat_log.txt";
        if (exportLog(path)) {
            return "Transcript successfully exported to " + path;
        } else {
            return "Failed to write transcript to " + path;
        }
    }

    return "Unknown command: " + cmd + ". Type /help for available commands.";
}

bool ChatEngine::exportLog(const std::string& filepath) const {
    std::ofstream out(filepath);
    if (!out.is_open()) return false;

    out << "=== OFFLINE CHAT LOG TRANSCRIPT ===\n";
    out << "Total Messages: " << m_history.size() << "\n";
    out << "-----------------------------------\n";
    for (const auto& msg : m_history) {
        out << "[" << msg.timestamp << "] " << msg.sender << ": " << msg.content << "\n";
    }
    out << "=== END OF TRANSCRIPT ===\n";
    return true;
}

} // namespace offline_chat
