#include "GameClient.hpp"
#include <iostream>
#include <sstream>
#include <cstring>

namespace lan_game {

GameClient::GameClient() {
    initGameSockets();
}

GameClient::~GameClient() {
    disconnect();
    cleanupGameSockets();
}

bool GameClient::connectToServer(const std::string& host, int port, const std::string& playerName) {
    if (m_connected.load()) disconnect();

    m_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (m_sock == INVALID_SOCK_FD) return false;

    sockaddr_in serverAddr{};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(static_cast<uint16_t>(port));

    if (inet_pton(AF_INET, host.c_str(), &serverAddr.sin_addr) <= 0) {
        closeGameSocket(m_sock);
        m_sock = INVALID_SOCK_FD;
        return false;
    }

    if (connect(m_sock, reinterpret_cast<sockaddr*>(&serverAddr), sizeof(serverAddr)) == SOCK_ERROR_CODE) {
        closeGameSocket(m_sock);
        m_sock = INVALID_SOCK_FD;
        return false;
    }

    // Send JOIN handshake
    std::string joinMsg = "JOIN " + (playerName.empty() ? "Player" : playerName) + "\n";
    if (send(m_sock, joinMsg.c_str(), static_cast<int>(joinMsg.size()), 0) <= 0) {
        closeGameSocket(m_sock);
        m_sock = INVALID_SOCK_FD;
        return false;
    }

    // Wait for WELCOME <pid>
    char buf[256];
    int n = recv(m_sock, buf, sizeof(buf) - 1, 0);
    if (n <= 0) {
        closeGameSocket(m_sock);
        m_sock = INVALID_SOCK_FD;
        return false;
    }
    buf[n] = '\0';
    std::istringstream iss(buf);
    std::string tag;
    if (!(iss >> tag >> m_localPlayerId) || tag != "WELCOME") {
        closeGameSocket(m_sock);
        m_sock = INVALID_SOCK_FD;
        return false;
    }

    m_connected.store(true);
    m_recvThread = std::thread(&GameClient::receiveWorker, this);
    return true;
}

void GameClient::disconnect() {
    if (!m_connected.exchange(false)) return;

    if (m_sock != INVALID_SOCK_FD) {
        std::string leaveMsg = "LEAVE\n";
        send(m_sock, leaveMsg.c_str(), static_cast<int>(leaveMsg.size()), 0);
        closeGameSocket(m_sock);
        m_sock = INVALID_SOCK_FD;
    }

    if (m_recvThread.joinable()) {
        m_recvThread.join();
    }
}

bool GameClient::isConnected() const {
    return m_connected.load();
}

bool GameClient::sendInput(float moveX, float moveY) {
    if (!m_connected.load() || m_sock == INVALID_SOCK_FD) return false;

    m_inputSeq++;
    std::ostringstream oss;
    oss << "INPUT " << m_inputSeq << " " << moveX << " " << moveY << "\n";
    std::string data = oss.str();
    int res = send(m_sock, data.c_str(), static_cast<int>(data.size()), 0);
    return res > 0;
}

std::optional<WorldSnapshot> GameClient::getLatestSnapshot() const {
    std::lock_guard<std::mutex> lock(m_snapMutex);
    return m_latestSnapshot;
}

int GameClient::getLocalPlayerId() const {
    return m_localPlayerId;
}

void GameClient::receiveWorker() {
    char buf[1024];
    std::string streamBuf;

    while (m_connected.load()) {
        int n = recv(m_sock, buf, sizeof(buf) - 1, 0);
        if (n <= 0) break;
        buf[n] = '\0';
        streamBuf.append(buf);

        size_t newlinePos = 0;
        while ((newlinePos = streamBuf.find('\n')) != std::string::npos) {
            std::string line = streamBuf.substr(0, newlinePos);
            streamBuf.erase(0, newlinePos + 1);

            auto snap = WorldSnapshot::deserialize(line);
            if (snap) {
                std::lock_guard<std::mutex> lock(m_snapMutex);
                m_latestSnapshot = snap;
            }
        }
    }
    m_connected.store(false);
}

} // namespace lan_game
