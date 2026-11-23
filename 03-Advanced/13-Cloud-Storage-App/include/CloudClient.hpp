#pragma once

#include "SocketCompat.hpp"
#include <string>
#include <vector>
#include <optional>
#include <cstdint>

namespace cloud_storage {

class CloudClient {
public:
    CloudClient();
    ~CloudClient();

    bool connectToServer(const std::string& host, int port);
    void disconnect();
    [[nodiscard]] bool isConnected() const noexcept;

    bool login(const std::string& username, const std::string& password);
    [[nodiscard]] const std::string& getAuthToken() const noexcept;

    bool uploadFile(const std::string& filename, const std::vector<uint8_t>& data, size_t chunkSize = 16384);
    std::optional<std::vector<uint8_t>> downloadFile(const std::string& filename);

private:
    std::string sendAndReceive(const std::string& message);

    socket_t m_sock{INVALID_SOCK_FD};
    bool m_connected{false};
    std::string m_token;
};

} // namespace cloud_storage
