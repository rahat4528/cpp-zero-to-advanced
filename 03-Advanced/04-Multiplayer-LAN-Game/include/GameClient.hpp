#pragma once

#include "SocketCompat.hpp"
#include "GameProtocol.hpp"
#include <string>
#include <thread>
#include <atomic>
#include <mutex>
#include <optional>

namespace lan_game {

class GameClient {
public:
    GameClient();
    ~GameClient();

    bool connectToServer(const std::string& host, int port, const std::string& playerName);
    void disconnect();
    [[nodiscard]] bool isConnected() const;

    bool sendInput(float moveX, float moveY);
    [[nodiscard]] std::optional<WorldSnapshot> getLatestSnapshot() const;
    [[nodiscard]] int getLocalPlayerId() const;

private:
    void receiveWorker();

    socket_t m_sock{INVALID_SOCK_FD};
    std::atomic<bool> m_connected{false};
    std::thread m_recvThread;
    int m_inputSeq{0};
    int m_localPlayerId{-1};

    mutable std::mutex m_snapMutex;
    std::optional<WorldSnapshot> m_latestSnapshot;
};

} // namespace lan_game
