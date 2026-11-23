#pragma once

#include "SocketCompat.hpp"
#include "ChatProtocol.hpp"
#include <string>
#include <functional>
#include <thread>
#include <atomic>

namespace chat {

class ChatClient {
public:
    using MessageCallback = std::function<void(const chat_protocol::ChatMessage&)>;

    ChatClient();
    ~ChatClient();

    bool connectToServer(const std::string& host, int port, const std::string& username);
    void disconnect();
    [[nodiscard]] bool isConnected() const;

    bool sendMessage(const std::string& text);
    bool sendWhisper(const std::string& recipient, const std::string& text);
    bool requestUserList();

    void setMessageCallback(MessageCallback cb);

private:
    void receiveLoop();

    socket_t m_sock{INVALID_SOCKET_VAL};
    std::string m_username;
    std::atomic<bool> m_connected{false};
    std::thread m_rxThread;
    MessageCallback m_onMessage;
};

} // namespace chat
