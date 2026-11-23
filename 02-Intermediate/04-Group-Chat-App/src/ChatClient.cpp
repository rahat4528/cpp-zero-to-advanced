#include "ChatClient.hpp"
#include <iostream>
#include <cstring>

namespace chat {

ChatClient::ChatClient() {
    initSockets();
}

ChatClient::~ChatClient() {
    disconnect();
    cleanupSockets();
}

bool ChatClient::connectToServer(const std::string& host, int port, const std::string& username) {
    if (m_connected) return true;

    m_username = username;
    m_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (m_sock == INVALID_SOCKET_VAL) {
        return false;
    }

    sockaddr_in servAddr{};
    servAddr.sin_family = AF_INET;
    servAddr.sin_port = htons(static_cast<uint16_t>(port));

    if (inet_pton(AF_INET, host.c_str(), &servAddr.sin_addr) <= 0) {
        // Try resolving hostname
        addrinfo hints{}, *res = nullptr;
        hints.ai_family = AF_INET;
        hints.ai_socktype = SOCK_STREAM;
        if (getaddrinfo(host.c_str(), std::to_string(port).c_str(), &hints, &res) != 0 || !res) {
            closeSocket(m_sock);
            m_sock = INVALID_SOCKET_VAL;
            return false;
        }
        memcpy(&servAddr, res->ai_addr, sizeof(servAddr));
        freeaddrinfo(res);
    }

    if (connect(m_sock, reinterpret_cast<sockaddr*>(&servAddr), sizeof(servAddr)) == SOCKET_ERROR_VAL) {
        closeSocket(m_sock);
        m_sock = INVALID_SOCKET_VAL;
        return false;
    }

    m_connected = true;

    // Send JOIN announcement
    chat_protocol::ChatMessage joinMsg{
        chat_protocol::MessageType::JOIN,
        m_username,
        "",
        ""
    };
    std::string raw = joinMsg.serialize();
    send(m_sock, raw.c_str(), static_cast<int>(raw.length()), 0);

    m_rxThread = std::thread(&ChatClient::receiveLoop, this);
    return true;
}

void ChatClient::disconnect() {
    if (!m_connected) return;
    m_connected = false;

    if (m_sock != INVALID_SOCKET_VAL) {
        chat_protocol::ChatMessage leaveMsg{
            chat_protocol::MessageType::LEAVE,
            m_username,
            "",
            ""
        };
        std::string raw = leaveMsg.serialize();
        send(m_sock, raw.c_str(), static_cast<int>(raw.length()), 0);

        closeSocket(m_sock);
        m_sock = INVALID_SOCKET_VAL;
    }

    if (m_rxThread.joinable()) {
        m_rxThread.join();
    }
}

bool ChatClient::isConnected() const {
    return m_connected;
}

bool ChatClient::sendMessage(const std::string& text) {
    if (!m_connected) return false;
    chat_protocol::ChatMessage msg{
        chat_protocol::MessageType::MSG,
        m_username,
        "",
        text
    };
    std::string raw = msg.serialize();
    return send(m_sock, raw.c_str(), static_cast<int>(raw.length()), 0) != SOCKET_ERROR_VAL;
}

bool ChatClient::sendWhisper(const std::string& recipient, const std::string& text) {
    if (!m_connected) return false;
    chat_protocol::ChatMessage msg{
        chat_protocol::MessageType::WHISPER,
        m_username,
        recipient,
        text
    };
    std::string raw = msg.serialize();
    return send(m_sock, raw.c_str(), static_cast<int>(raw.length()), 0) != SOCKET_ERROR_VAL;
}

bool ChatClient::requestUserList() {
    if (!m_connected) return false;
    chat_protocol::ChatMessage msg{
        chat_protocol::MessageType::USERS,
        m_username,
        "",
        ""
    };
    std::string raw = msg.serialize();
    return send(m_sock, raw.c_str(), static_cast<int>(raw.length()), 0) != SOCKET_ERROR_VAL;
}

void ChatClient::setMessageCallback(MessageCallback cb) {
    m_onMessage = std::move(cb);
}

void ChatClient::receiveLoop() {
    char buffer[2048];
    std::string streamBuffer;

    while (m_connected) {
        int bytes = recv(m_sock, buffer, sizeof(buffer) - 1, 0);
        if (bytes <= 0) {
            break;
        }
        buffer[bytes] = '\0';
        streamBuffer.append(buffer, bytes);

        size_t newlinePos;
        while ((newlinePos = streamBuffer.find('\n')) != std::string::npos) {
            std::string line = streamBuffer.substr(0, newlinePos);
            streamBuffer.erase(0, newlinePos + 1);

            auto optMsg = chat_protocol::ChatMessage::deserialize(line);
            if (optMsg && m_onMessage) {
                m_onMessage(*optMsg);
            }
        }
    }
    m_connected = false;
}

} // namespace chat
