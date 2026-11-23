#include "BankingClient.hpp"
#include <sstream>
#include <iostream>

namespace banking {

BankingClient::BankingClient() {
    initBankingSockets();
}

BankingClient::~BankingClient() {
    disconnect();
    cleanupBankingSockets();
}

bool BankingClient::connectToServer(const std::string& host, int port) {
    disconnect();

    m_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (m_sock == INVALID_SOCK_FD) return false;

    sockaddr_in serverAddr{};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(static_cast<uint16_t>(port));

    if (inet_pton(AF_INET, host.c_str(), &serverAddr.sin_addr) <= 0) {
        closeBankingSocket(m_sock);
        m_sock = INVALID_SOCK_FD;
        return false;
    }

    if (connect(m_sock, reinterpret_cast<sockaddr*>(&serverAddr), sizeof(serverAddr)) == SOCK_ERROR_CODE) {
        closeBankingSocket(m_sock);
        m_sock = INVALID_SOCK_FD;
        return false;
    }

    m_connected = true;
    return true;
}

void BankingClient::disconnect() {
    if (m_connected) {
        if (m_sock != INVALID_SOCK_FD) {
            std::string q = "QUIT\n";
            send(m_sock, q.c_str(), static_cast<int>(q.size()), 0);
            closeBankingSocket(m_sock);
            m_sock = INVALID_SOCK_FD;
        }
        m_connected = false;
        m_authenticated = false;
    }
}

bool BankingClient::isConnected() const noexcept {
    return m_connected;
}

std::string BankingClient::sendAndReceive(const std::string& message) {
    if (!m_connected || m_sock == INVALID_SOCK_FD) return "";

    std::string term = message.back() == '\n' ? message : (message + "\n");
    if (send(m_sock, term.c_str(), static_cast<int>(term.size()), 0) <= 0) {
        return "";
    }

    std::string response;
    char c = 0;
    while (recv(m_sock, &c, 1, 0) == 1) {
        if (c == '\n') break;
        response += c;
    }
    if (!response.empty() && response.back() == '\r') response.pop_back();
    return response;
}

bool BankingClient::login(const std::string& accountNum, const std::string& pin) {
    std::string resp = sendAndReceive("LOGIN " + accountNum + " " + pin);
    std::istringstream iss(resp);
    std::string tag;
    if (iss >> tag && tag == "AUTH_OK") {
        m_authenticated = true;
        return true;
    }
    return false;
}

std::optional<int64_t> BankingClient::queryBalance() {
    if (!m_authenticated) return std::nullopt;

    std::string resp = sendAndReceive("BALANCE");
    std::istringstream iss(resp);
    std::string tag;
    int64_t cents = 0;
    if (iss >> tag >> cents && tag == "BALANCE_OK") {
        return cents;
    }
    return std::nullopt;
}

bool BankingClient::transferFunds(const std::string& toAccount, int64_t amountCents, std::string& outRef, std::string& outErr) {
    if (!m_authenticated) {
        outErr = "Not authenticated";
        return false;
    }

    std::string cmd = "TRANSFER " + toAccount + " " + std::to_string(amountCents);
    std::string resp = sendAndReceive(cmd);

    std::istringstream iss(resp);
    std::string tag;
    if (iss >> tag && tag == "TRANSFER_OK") {
        iss >> outRef;
        return true;
    }

    // Capture error message
    std::string errPart;
    std::getline(iss, errPart);
    outErr = errPart.empty() ? resp : errPart;
    return false;
}

} // namespace banking
