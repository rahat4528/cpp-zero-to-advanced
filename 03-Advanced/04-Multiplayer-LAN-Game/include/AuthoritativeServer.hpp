#pragma once

#include "SocketCompat.hpp"
#include "GameProtocol.hpp"
#include <map>
#include <mutex>
#include <thread>
#include <atomic>
#include <memory>

namespace lan_game {

class AuthoritativeServer {
public:
    explicit AuthoritativeServer(int port = 9000);
    ~AuthoritativeServer();

    bool start();
    void stop();
    [[nodiscard]] bool isRunning() const;

    void updatePhysicsTick();
    [[nodiscard]] WorldSnapshot getLatestSnapshot() const;
    [[nodiscard]] size_t getPlayerCount() const;

    void applyPlayerInput(int playerId, const PlayerInput& input);
    int registerPlayer(const std::string& name);
    void unregisterPlayer(int playerId);

private:
    void networkAcceptLoop();
    void clientWorker(socket_t clientSock, int playerId);
    void broadcastSnapshot();

    int m_port;
    socket_t m_serverSock{INVALID_SOCK_FD};
    std::atomic<bool> m_running{false};
    std::thread m_acceptThread;
    std::thread m_tickThread;

    mutable std::mutex m_stateMutex;
    uint32_t m_currentTick{0};
    int m_nextPlayerId{1};
    std::map<int, PlayerState> m_players;

    struct ClientSession {
        socket_t sock;
        int playerId;
        std::thread worker;
    };
    std::map<int, std::shared_ptr<ClientSession>> m_sessions;
};

} // namespace lan_game
