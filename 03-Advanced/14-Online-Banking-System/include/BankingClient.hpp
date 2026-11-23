#pragma once

#include "SocketCompat.hpp"
#include <string>
#include <optional>
#include <cstdint>

namespace banking {

class BankingClient {
public:
    BankingClient();
    ~BankingClient();

    bool connectToServer(const std::string& host, int port);
    void disconnect();
    [[nodiscard]] bool isConnected() const noexcept;

    bool login(const std::string& accountNum, const std::string& pin);
    [[nodiscard]] std::optional<int64_t> queryBalance();
    bool transferFunds(const std::string& toAccount, int64_t amountCents, std::string& outRef, std::string& outErr);

private:
    std::string sendAndReceive(const std::string& message);

    socket_t m_sock{INVALID_SOCK_FD};
    bool m_connected{false};
    bool m_authenticated{false};
};

} // namespace banking
