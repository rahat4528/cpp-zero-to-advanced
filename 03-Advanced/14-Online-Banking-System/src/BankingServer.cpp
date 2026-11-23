#include "BankingServer.hpp"
#include <iostream>
#include <sstream>

namespace banking {

BankingServer::BankingServer(int port, const std::string& dbPath)
    : m_port(port), m_db(dbPath) {
    initBankingSockets();
    m_db.open();
}

BankingServer::~BankingServer() {
    stop();
    cleanupBankingSockets();
}

bool BankingServer::start() {
    if (m_running.load()) return true;

    m_serverSock = socket(AF_INET, SOCK_STREAM, 0);
    if (m_serverSock == INVALID_SOCK_FD) return false;

    int opt = 1;
    setsockopt(m_serverSock, SOL_SOCKET, SO_REUSEADDR, reinterpret_cast<const char*>(&opt), sizeof(opt));

    sockaddr_in serverAddr{};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(static_cast<uint16_t>(m_port));

    if (bind(m_serverSock, reinterpret_cast<sockaddr*>(&serverAddr), sizeof(serverAddr)) == SOCK_ERROR_CODE) {
        closeBankingSocket(m_serverSock);
        m_serverSock = INVALID_SOCK_FD;
        return false;
    }

    if (listen(m_serverSock, 10) == SOCK_ERROR_CODE) {
        closeBankingSocket(m_serverSock);
        m_serverSock = INVALID_SOCK_FD;
        return false;
    }

    m_running.store(true);
    m_acceptThread = std::thread(&BankingServer::acceptLoop, this);
    return true;
}

void BankingServer::stop() {
    if (!m_running.exchange(false)) return;

    if (m_serverSock != INVALID_SOCK_FD) {
        closeBankingSocket(m_serverSock);
        m_serverSock = INVALID_SOCK_FD;
    }

    if (m_acceptThread.joinable()) {
        m_acceptThread.join();
    }

    std::lock_guard<std::mutex> lock(m_threadMutex);
    for (auto& t : m_workerThreads) {
        if (t.joinable()) t.join();
    }
    m_workerThreads.clear();
}

bool BankingServer::isRunning() const noexcept {
    return m_running.load();
}

void BankingServer::acceptLoop() {
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

        std::lock_guard<std::mutex> lock(m_threadMutex);
        m_workerThreads.emplace_back(&BankingServer::clientWorker, this, clientSock);
    }
}

void BankingServer::clientWorker(socket_t clientSock) {
    char buf[2048];
    std::string streamBuf;
    std::string currentAccount;

    auto sendResp = [clientSock](const std::string& msg) {
        std::string term = msg.back() == '\n' ? msg : (msg + "\n");
        send(clientSock, term.c_str(), static_cast<int>(term.size()), 0);
    };

    while (m_running.load()) {
        int n = recv(clientSock, buf, sizeof(buf) - 1, 0);
        if (n <= 0) break;
        buf[n] = '\0';
        streamBuf.append(buf);

        size_t newlinePos = 0;
        while ((newlinePos = streamBuf.find('\n')) != std::string::npos) {
            std::string line = streamBuf.substr(0, newlinePos);
            streamBuf.erase(0, newlinePos + 1);
            if (!line.empty() && line.back() == '\r') line.pop_back();

            std::istringstream iss(line);
            std::string cmd;
            if (!(iss >> cmd)) continue;

            if (cmd == "LOGIN") {
                std::string accNum, pin;
                iss >> accNum >> pin;
                auto accOpt = m_db.authenticate(accNum, pin);
                if (accOpt.has_value()) {
                    currentAccount = accNum;
                    sendResp("AUTH_OK " + accOpt->username + " " + std::to_string(accOpt->balanceCents));
                } else {
                    sendResp("ERROR Invalid_Account_Or_PIN");
                }
            } else if (cmd == "BALANCE") {
                if (currentAccount.empty()) {
                    sendResp("ERROR Unauthorized");
                    continue;
                }
                auto bal = m_db.getBalance(currentAccount);
                if (bal) {
                    sendResp("BALANCE_OK " + std::to_string(*bal));
                } else {
                    sendResp("ERROR Account_Not_Found");
                }
            } else if (cmd == "TRANSFER") {
                if (currentAccount.empty()) {
                    sendResp("ERROR Unauthorized");
                    continue;
                }
                std::string toAcc;
                int64_t amount = 0;
                iss >> toAcc >> amount;

                std::string ref, err;
                if (m_db.executeTransfer(currentAccount, toAcc, amount, ref, err)) {
                    sendResp("TRANSFER_OK " + ref);
                } else {
                    sendResp("ERROR " + err);
                }
            } else if (cmd == "QUIT") {
                goto end_worker;
            }
        }
    }

end_worker:
    closeBankingSocket(clientSock);
}

} // namespace banking
