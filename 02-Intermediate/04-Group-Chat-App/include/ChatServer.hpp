#pragma once

#include "SocketCompat.hpp"
#include "ChatProtocol.hpp"
#include <string>
#include <vector>
#include <map>
#include <mutex>
#include <atomic>
#include <thread>
#include <memory>

namespace chat {

class ChatServer {
public:
    explicit ChatServer(int port = 8080);
    ~ChatServer();

    bool start();
    void stop();
    [[nodiscard]] bool isRunning() const;
    [[nodiscard]] size_t getConnectedClientCount() const;

private:
    void acceptLoop();
    void handleClient(socket_t clientSock);
    void broadcast(const chat_protocol::ChatMessage& msg, socket_t excludeSock = INVALID_SOCKET_VAL);
    void sendDirect(socket_t sock, const chat_protocol::ChatMessage& msg);
    void removeClient(socket_t sock);

    int m_port;
    socket_t m_serverSock{INVALID_SOCKET_VAL};
    std::atomic<bool> m_running{false};
    std::thread m_acceptThread;

    mutable std::mutex m_clientsMutex;
    struct ClientSession {
        socket_t sock;
        std::string username;
        std::thread worker;
    };
    std::map<socket_t, std::shared_ptr<ClientSession>> m_clients;
};

} // namespace chat
