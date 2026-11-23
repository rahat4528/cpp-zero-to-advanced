#pragma once

#include <chrono>
#include <string>
#include <vector>

namespace offline_chat {

struct ChatMessage {
    std::string sender;
    std::string content;
    std::string timestamp; // Formatted ISO or HH:MM:SS
};

enum class ChatMode {
    Bot,
    TwoUser
};

class ChatEngine {
public:
    explicit ChatEngine(ChatMode mode = ChatMode::Bot);

    // Sends a message from the current user or participant
    ChatMessage sendMessage(const std::string& sender, const std::string& text);

    // Bot response generator based on conversational heuristics
    std::string generateBotResponse(const std::string& userPrompt) const;

    // Command handling (returns response text or empty if normal message)
    bool isCommand(const std::string& text) const;
    std::string executeCommand(const std::string& commandLine);

    // Session history
    const std::vector<ChatMessage>& getHistory() const { return m_history; }
    void clearHistory() { m_history.clear(); }

    // Export log to disk
    bool exportLog(const std::string& filepath) const;

    // Helper timestamp generator
    static std::string getCurrentTimestamp();

    ChatMode getMode() const { return m_mode; }
    void setMode(ChatMode mode) { m_mode = mode; }

private:
    ChatMode m_mode;
    std::vector<ChatMessage> m_history;
};

} // namespace offline_chat
