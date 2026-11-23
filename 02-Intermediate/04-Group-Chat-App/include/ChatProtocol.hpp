#pragma once

#include <string>
#include <sstream>
#include <vector>
#include <optional>

namespace chat_protocol {

enum class MessageType {
    JOIN,       // User joining: "JOIN|username"
    MSG,        // Broadcast message: "MSG|sender|text"
    WHISPER,    // Private message: "WHISPER|sender|recipient|text"
    USERS,      // Request or response for user list: "USERS|user1,user2,..."
    LEAVE,      // User leaving: "LEAVE|username"
    SYSTEM      // System alert: "SYSTEM|server|text"
};

struct ChatMessage {
    MessageType type{MessageType::MSG};
    std::string sender;
    std::string recipient; // empty for public broadcast
    std::string text;

    [[nodiscard]] std::string serialize() const {
        std::ostringstream oss;
        switch (type) {
            case MessageType::JOIN:
                oss << "JOIN|" << sender;
                break;
            case MessageType::MSG:
                oss << "MSG|" << sender << "|" << text;
                break;
            case MessageType::WHISPER:
                oss << "WHISPER|" << sender << "|" << recipient << "|" << text;
                break;
            case MessageType::USERS:
                oss << "USERS|" << text;
                break;
            case MessageType::LEAVE:
                oss << "LEAVE|" << sender;
                break;
            case MessageType::SYSTEM:
                oss << "SYSTEM|" << sender << "|" << text;
                break;
        }
        oss << "\n";
        return oss.str();
    }

    static std::optional<ChatMessage> deserialize(const std::string& raw) {
        if (raw.empty()) return std::nullopt;

        // Remove trailing \r or \n
        std::string line = raw;
        while (!line.empty() && (line.back() == '\r' || line.back() == '\n')) {
            line.pop_back();
        }

        std::vector<std::string> parts;
        size_t start = 0;
        while (start < line.length()) {
            size_t delim = line.find('|', start);
            if (delim == std::string::npos) {
                parts.push_back(line.substr(start));
                break;
            }
            parts.push_back(line.substr(start, delim - start));
            start = delim + 1;
        }

        if (parts.empty()) return std::nullopt;

        ChatMessage msg;
        const auto& tag = parts[0];

        if (tag == "JOIN" && parts.size() >= 2) {
            msg.type = MessageType::JOIN;
            msg.sender = parts[1];
            return msg;
        } else if (tag == "MSG" && parts.size() >= 3) {
            msg.type = MessageType::MSG;
            msg.sender = parts[1];
            // Reconstruct text in case message had '|' characters
            std::string body = parts[2];
            for (size_t i = 3; i < parts.size(); ++i) {
                body += "|" + parts[i];
            }
            msg.text = body;
            return msg;
        } else if (tag == "WHISPER" && parts.size() >= 4) {
            msg.type = MessageType::WHISPER;
            msg.sender = parts[1];
            msg.recipient = parts[2];
            std::string body = parts[3];
            for (size_t i = 4; i < parts.size(); ++i) {
                body += "|" + parts[i];
            }
            msg.text = body;
            return msg;
        } else if (tag == "USERS" && parts.size() >= 2) {
            msg.type = MessageType::USERS;
            msg.text = parts[1];
            return msg;
        } else if (tag == "LEAVE" && parts.size() >= 2) {
            msg.type = MessageType::LEAVE;
            msg.sender = parts[1];
            return msg;
        } else if (tag == "SYSTEM" && parts.size() >= 3) {
            msg.type = MessageType::SYSTEM;
            msg.sender = parts[1];
            msg.text = parts[2];
            return msg;
        }

        return std::nullopt;
    }
};

} // namespace chat_protocol
