#pragma once

#include "SocketCompat.hpp"
#include "BankingDatabase.hpp"
#include <string>
#include <vector>
#include <thread>
#include <atomic>
#include <mutex>

namespace banking {

class BankingServer {
public:
    explicit BankingServer(int port = 9050, const std::string& dbPath = ":memory:");
    ~BankingServer();

    bool start();
    void stop();
    [[nodiscard]] bool isRunning() const noexcept;

    BankingDatabase& getDatabase() noexcept { return m_db; }
    [[nodiscard]] const BankingDatabase& getDatabase() const noexcept { return m_db; }

private:
    void acceptLoop();
    void clientWorker(socket_t clientSock);

    int m_port;
    BankingDatabase m_db;
    socket_t m_serverSock{INVALID_SOCK_FD};
    std::atomic<bool> m_running{false};
    std::thread m_acceptThread;
    std::vector<std::thread> m_workerThreads;
    std::mutex m_threadMutex;
};

} // namespace banking
