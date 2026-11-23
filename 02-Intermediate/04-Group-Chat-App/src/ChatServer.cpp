#include "ChatServer.hpp"
#include <iostream>
#include <vector>
#include <cstring>

namespace chat {

ChatServer::ChatServer(int port) : m_port(port) {
    initSockets();
}

ChatServer::~ChatServer() {
    stop();
    cleanupSockets();
}

bool ChatServer::start() {
    if (m_running) return true;

    m_serverSock = socket(AF_INET, SOCK_STREAM, 0);
    if (m_serverSock == INVALID_SOCKET_VAL) {
        std::cerr << "[Server] Failed to create socket\n";
        return false;
    }

    int opt = 1;
    setsockopt(m_serverSock, SOL_SOCKET, SO_REUSEADDR, reinterpret_cast<const char*>(&opt), sizeof(opt));

    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(static_cast<uint16_t>(m_port));

    if (bind(m_serverSock, reinterpret_cast<sockaddr*>(&addr), sizeof(addr)) == SOCKET_ERROR_VAL) {
        std::cerr << "[Server] Bind failed on port " << m_port << "\n";
        closeSocket(m_serverSock);
        m_serverSock = INVALID_SOCKET_VAL;
        return false;
    }

    if (listen(m_serverSock, 10) == SOCKET_ERROR_VAL) {
        std::cerr << "[Server] Listen failed\n";
        closeSocket(m_serverSock);
        m_serverSock = INVALID_SOCKET_VAL;
        return false;
    }

    m_running = true;
    m_acceptThread = std::thread(&ChatServer::acceptLoop, this);
    std::cout << "[Server] Group Chat Server listening on port " << m_port << "...\n";
    return true;
}

void ChatServer::stop() {
    if (!m_running) return;
    m_running = false;

    if (m_serverSock != INVALID_SOCKET_VAL) {
        closeSocket(m_serverSock);
        m_serverSock = INVALID_SOCKET_VAL;
    }

    if (m_acceptThread.joinable()) {
        m_acceptThread.join();
    }

    std::lock_guard<std::mutex> lock(m_clientsMutex);
    for (auto& [sock, session] : m_clients) {
        closeSocket(sock);
        if (session->worker.joinable()) {
            session->worker.detach();
        }
    }
    m_clients.clear();
    std::cout << "[Server] Stopped.\n";
}

bool ChatServer::isRunning() const {
    return m_running;
}

size_t ChatServer::getConnectedClientCount() const {
    std::lock_guard<std::mutex> lock(m_clientsMutex);
    return m_clients.size();
}

void ChatServer::acceptLoop() {
    while (m_running) {
        sockaddr_in clientAddr{};
        int addrLen = sizeof(clientAddr);
        socket_t clientSock = accept(m_serverSock, reinterpret_cast<sockaddr*>(&clientAddr), &addrLen);

        if (clientSock == INVALID_SOCKET_VAL) {
            if (!m_running) break;
            continue;
        }

        auto session = std::make_shared<ClientSession>();
        session->sock = clientSock;
        session->username = "Anonymous";

        {
            std::lock_guard<std::mutex> lock(m_clientsMutex);
            m_clients[clientSock] = session;
        }

        session->worker = std::thread(&ChatServer::handleClient, this, clientSock);
    }
}

void ChatServer::handleClient(socket_t clientSock) {
    char buffer[2048];
    std::string streamBuffer;

    while (m_running) {
        int bytes = recv(clientSock, buffer, sizeof(buffer) - 1, 0);
        if (bytes <= 0) {
            break; // Disconnected
        }
        buffer[bytes] = '\0';
        streamBuffer.append(buffer, bytes);

        size_t newlinePos;
        while ((newlinePos = streamBuffer.find('\n')) != std::string::npos) {
            std::string line = streamBuffer.substr(0, newlinePos);
            streamBuffer.erase(0, newlinePos + 1);

            auto optMsg = chat_protocol::ChatMessage::deserialize(line);
            if (!optMsg) continue;

            const auto& msg = *optMsg;
            if (msg.type == chat_protocol::MessageType::JOIN) {
                {
                    std::lock_guard<std::mutex> lock(m_clientsMutex);
                    if (m_clients.count(clientSock)) {
                        m_clients[clientSock]->username = msg.sender;
                    }
                }
                chat_protocol::ChatMessage alert{
                    chat_protocol::MessageType::SYSTEM,
                    "Server",
                    "",
                    msg.sender + " has entered the chat room."
                };
                broadcast(alert);
            } else if (msg.type == chat_protocol::MessageType::MSG) {
                broadcast(msg);
            } else if (msg.type == chat_protocol::MessageType::WHISPER) {
                // Direct message to recipient
                socket_t recipientSock = INVALID_SOCKET_VAL;
                {
                    std::lock_guard<std::mutex> lock(m_clientsMutex);
                    for (const auto& [s, sess] : m_clients) {
                        if (sess->username == msg.recipient) {
                            recipientSock = s;
                            break;
                        }
                    }
                }
                if (recipientSock != INVALID_SOCKET_VAL) {
                    sendDirect(recipientSock, msg);
                    sendDirect(clientSock, msg); // Echo back to sender
                } else {
                    chat_protocol::ChatMessage notFound{
                        chat_protocol::MessageType::SYSTEM,
                        "Server",
                        "",
                        "User '" + msg.recipient + "' not found."
                    };
                    sendDirect(clientSock, notFound);
                }
            } else if (msg.type == chat_protocol::MessageType::USERS) {
                std::string listStr;
                {
                    std::lock_guard<std::mutex> lock(m_clientsMutex);
                    for (const auto& [s, sess] : m_clients) {
                        if (!listStr.empty()) listStr += ", ";
                        listStr += sess->username;
                    }
                }
                chat_protocol::ChatMessage userList{
                    chat_protocol::MessageType::USERS,
                    "Server",
                    "",
                    listStr
                };
                sendDirect(clientSock, userList);
            } else if (msg.type == chat_protocol::MessageType::LEAVE) {
                break;
            }
        }
    }

    removeClient(clientSock);
}

void ChatServer::broadcast(const chat_protocol::ChatMessage& msg, socket_t excludeSock) {
    std::string raw = msg.serialize();
    std::lock_guard<std::mutex> lock(m_clientsMutex);
    for (const auto& [sock, sess] : m_clients) {
        if (sock != excludeSock) {
            send(sock, raw.c_str(), static_cast<int>(raw.length()), 0);
        }
    }
}

void ChatServer::sendDirect(socket_t sock, const chat_protocol::ChatMessage& msg) {
    std::string raw = msg.serialize();
    send(sock, raw.c_str(), static_cast<int>(raw.length()), 0);
}

void ChatServer::removeClient(socket_t sock) {
    std::string departedUser;
    {
        std::lock_guard<std::mutex> lock(m_clientsMutex);
        if (m_clients.count(sock)) {
            departedUser = m_clients[sock]->username;
            if (m_clients[sock]->worker.joinable()) {
                m_clients[sock]->worker.detach();
            }
            closeSocket(sock);
            m_clients.erase(sock);
        }
    }

    if (!departedUser.empty() && departedUser != "Anonymous") {
        chat_protocol::ChatMessage leaveMsg{
            chat_protocol::MessageType::SYSTEM,
            "Server",
            "",
            departedUser + " has left the chat room."
        };
        broadcast(leaveMsg);
    }
}

} // namespace chat
