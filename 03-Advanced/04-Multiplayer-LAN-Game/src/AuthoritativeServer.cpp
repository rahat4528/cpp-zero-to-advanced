#include "AuthoritativeServer.hpp"
#include <iostream>
#include <algorithm>
#include <chrono>
#include <cstring>

namespace lan_game {

AuthoritativeServer::AuthoritativeServer(int port)
    : m_port(port) {
    initGameSockets();
}

AuthoritativeServer::~AuthoritativeServer() {
    stop();
    cleanupGameSockets();
}

bool AuthoritativeServer::start() {
    if (m_running.load()) return true;

    m_serverSock = socket(AF_INET, SOCK_STREAM, 0);
    if (m_serverSock == INVALID_SOCK_FD) {
        return false;
    }

    int opt = 1;
    setsockopt(m_serverSock, SOL_SOCKET, SO_REUSEADDR, reinterpret_cast<const char*>(&opt), sizeof(opt));

    sockaddr_in serverAddr{};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(static_cast<uint16_t>(m_port));

    if (bind(m_serverSock, reinterpret_cast<sockaddr*>(&serverAddr), sizeof(serverAddr)) == SOCK_ERROR_CODE) {
        closeGameSocket(m_serverSock);
        m_serverSock = INVALID_SOCK_FD;
        return false;
    }

    if (listen(m_serverSock, 8) == SOCK_ERROR_CODE) {
        closeGameSocket(m_serverSock);
        m_serverSock = INVALID_SOCK_FD;
        return false;
    }

    m_running.store(true);

    m_acceptThread = std::thread(&AuthoritativeServer::networkAcceptLoop, this);
    m_tickThread = std::thread([this]() {
        while (m_running.load()) {
            std::this_thread::sleep_for(std::chrono::milliseconds(50)); // 20 Hz
            updatePhysicsTick();
            broadcastSnapshot();
        }
    });

    return true;
}

void AuthoritativeServer::stop() {
    if (!m_running.exchange(false)) return;

    if (m_serverSock != INVALID_SOCK_FD) {
        closeGameSocket(m_serverSock);
        m_serverSock = INVALID_SOCK_FD;
    }

    if (m_acceptThread.joinable()) {
        m_acceptThread.join();
    }
    if (m_tickThread.joinable()) {
        m_tickThread.join();
    }

    std::lock_guard<std::mutex> lock(m_stateMutex);
    for (auto& [id, session] : m_sessions) {
        if (session && session->sock != INVALID_SOCK_FD) {
            closeGameSocket(session->sock);
            session->sock = INVALID_SOCK_FD;
            if (session->worker.joinable()) {
                session->worker.join();
            }
        }
    }
    m_sessions.clear();
    m_players.clear();
}

bool AuthoritativeServer::isRunning() const {
    return m_running.load();
}

int AuthoritativeServer::registerPlayer(const std::string& name) {
    std::lock_guard<std::mutex> lock(m_stateMutex);
    int id = m_nextPlayerId++;
    PlayerState state;
    state.id = id;
    state.name = name.empty() ? ("Player" + std::to_string(id)) : name;
    state.x = 100.0f + static_cast<float>((id * 37) % 600);
    state.y = 100.0f + static_cast<float>((id * 53) % 400);
    state.health = 100;
    state.score = 0;
    m_players[id] = state;
    return id;
}

void AuthoritativeServer::unregisterPlayer(int playerId) {
    std::lock_guard<std::mutex> lock(m_stateMutex);
    m_players.erase(playerId);
}

size_t AuthoritativeServer::getPlayerCount() const {
    std::lock_guard<std::mutex> lock(m_stateMutex);
    return m_players.size();
}

void AuthoritativeServer::applyPlayerInput(int playerId, const PlayerInput& input) {
    std::lock_guard<std::mutex> lock(m_stateMutex);
    auto it = m_players.find(playerId);
    if (it != m_players.end()) {
        constexpr float SPEED = 5.0f;
        it->second.x += input.moveX * SPEED;
        it->second.y += input.moveY * SPEED;

        // Clamp boundaries: [0, 800] x [0, 600]
        it->second.x = std::clamp(it->second.x, 0.0f, 800.0f);
        it->second.y = std::clamp(it->second.y, 0.0f, 600.0f);
    }
}

void AuthoritativeServer::updatePhysicsTick() {
    std::lock_guard<std::mutex> lock(m_stateMutex);
    m_currentTick++;
    // Physics rules: collision or score increment when near center (400, 300)
    for (auto& [id, player] : m_players) {
        float dx = player.x - 400.0f;
        float dy = player.y - 300.0f;
        if (dx * dx + dy * dy < 2500.0f) { // Within 50 units of center
            player.score += 1;
        }
    }
}

WorldSnapshot AuthoritativeServer::getLatestSnapshot() const {
    std::lock_guard<std::mutex> lock(m_stateMutex);
    WorldSnapshot snap;
    snap.tick = m_currentTick;
    for (const auto& [id, player] : m_players) {
        snap.players.push_back(player);
    }
    return snap;
}

void AuthoritativeServer::broadcastSnapshot() {
    WorldSnapshot snap = getLatestSnapshot();
    std::string data = snap.serialize();

    std::lock_guard<std::mutex> lock(m_stateMutex);
    for (auto& [id, session] : m_sessions) {
        if (session && session->sock != INVALID_SOCK_FD) {
            send(session->sock, data.c_str(), static_cast<int>(data.size()), 0);
        }
    }
}

void AuthoritativeServer::networkAcceptLoop() {
    while (m_running.load()) {
        sockaddr_in clientAddr{};
#if defined(_WIN32) || defined(_WIN64)
        int addrLen = sizeof(clientAddr);
        socket_t clientSock = accept(m_serverSock, reinterpret_cast<sockaddr*>(&clientAddr), &addrLen);
#else
        socklen_t addrLen = sizeof(clientAddr);
        socket_t clientSock = accept(m_serverSock, reinterpret_cast<sockaddr*>(&clientAddr), &addrLen);
#endif
        if (clientSock == INVALID_SOCK_FD) {
            if (!m_running.load()) break;
            continue;
        }

        // Read initial greeting: JOIN <name>\n
        char buf[256];
        int n = recv(clientSock, buf, sizeof(buf) - 1, 0);
        if (n <= 0) {
            closeGameSocket(clientSock);
            continue;
        }
        buf[n] = '\0';
        std::string line(buf);
        std::istringstream iss(line);
        std::string cmd, name;
        iss >> cmd >> name;
        if (cmd != "JOIN") {
            closeGameSocket(clientSock);
            continue;
        }

        int pid = registerPlayer(name);
        std::string welcome = "WELCOME " + std::to_string(pid) + "\n";
        send(clientSock, welcome.c_str(), static_cast<int>(welcome.size()), 0);

        auto session = std::make_shared<ClientSession>();
        session->sock = clientSock;
        session->playerId = pid;
        session->worker = std::thread(&AuthoritativeServer::clientWorker, this, clientSock, pid);

        {
            std::lock_guard<std::mutex> lock(m_stateMutex);
            m_sessions[pid] = session;
        }
    }
}

void AuthoritativeServer::clientWorker(socket_t clientSock, int playerId) {
    char buf[512];
    std::string streamBuf;

    while (m_running.load()) {
        int n = recv(clientSock, buf, sizeof(buf) - 1, 0);
        if (n <= 0) break;
        buf[n] = '\0';
        streamBuf.append(buf);

        size_t newlinePos = 0;
        while ((newlinePos = streamBuf.find('\n')) != std::string::npos) {
            std::string line = streamBuf.substr(0, newlinePos);
            streamBuf.erase(0, newlinePos + 1);

            std::istringstream iss(line);
            std::string cmd;
            if (iss >> cmd) {
                if (cmd == "INPUT") {
                    PlayerInput inp;
                    if (iss >> inp.sequence >> inp.moveX >> inp.moveY) {
                        applyPlayerInput(playerId, inp);
                    }
                } else if (cmd == "LEAVE") {
                    goto cleanup_exit;
                }
            }
        }
    }

cleanup_exit:
    unregisterPlayer(playerId);
}

} // namespace lan_game
