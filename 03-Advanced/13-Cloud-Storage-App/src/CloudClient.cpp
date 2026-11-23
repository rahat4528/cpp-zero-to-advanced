#include "CloudClient.hpp"
#include "CloudProtocol.hpp"
#include <iostream>
#include <sstream>
#include <cstring>

namespace cloud_storage {

CloudClient::CloudClient() {
    initCloudSockets();
}

CloudClient::~CloudClient() {
    disconnect();
    cleanupCloudSockets();
}

bool CloudClient::connectToServer(const std::string& host, int port) {
    disconnect();

    m_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (m_sock == INVALID_SOCK_FD) return false;

    sockaddr_in serverAddr{};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(static_cast<uint16_t>(port));

    if (inet_pton(AF_INET, host.c_str(), &serverAddr.sin_addr) <= 0) {
        closeCloudSocket(m_sock);
        m_sock = INVALID_SOCK_FD;
        return false;
    }

    if (connect(m_sock, reinterpret_cast<sockaddr*>(&serverAddr), sizeof(serverAddr)) == SOCK_ERROR_CODE) {
        closeCloudSocket(m_sock);
        m_sock = INVALID_SOCK_FD;
        return false;
    }

    m_connected = true;
    return true;
}

void CloudClient::disconnect() {
    if (m_connected) {
        if (m_sock != INVALID_SOCK_FD) {
            std::string q = "QUIT\n";
            send(m_sock, q.c_str(), static_cast<int>(q.size()), 0);
            closeCloudSocket(m_sock);
            m_sock = INVALID_SOCK_FD;
        }
        m_connected = false;
        m_token.clear();
    }
}

bool CloudClient::isConnected() const noexcept {
    return m_connected;
}

const std::string& CloudClient::getAuthToken() const noexcept {
    return m_token;
}

std::string CloudClient::sendAndReceive(const std::string& message) {
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

bool CloudClient::login(const std::string& username, const std::string& password) {
    std::string resp = sendAndReceive("LOGIN " + username + " " + password);
    std::istringstream iss(resp);
    std::string tag, token;
    if (iss >> tag >> token && tag == "TOKEN") {
        m_token = token;
        return true;
    }
    return false;
}

bool CloudClient::uploadFile(const std::string& filename, const std::vector<uint8_t>& data, size_t chunkSize) {
    if (m_token.empty()) return false;

    std::string fileHash = computeSha256(data);
    std::ostringstream initCmd;
    initCmd << "UPLOAD_INIT " << m_token << " " << filename << " " << data.size() << " " << fileHash;
    std::string initResp = sendAndReceive(initCmd.str());

    std::istringstream iss(initResp);
    std::string tag, uploadId;
    if (!(iss >> tag >> uploadId) || tag != "UPLOAD_OK") {
        return false;
    }

    size_t offset = 0;
    while (offset < data.size()) {
        size_t currentChunkSize = std::min(chunkSize, data.size() - offset);
        std::vector<uint8_t> chunk(data.begin() + offset, data.begin() + offset + currentChunkSize);
        std::string hexData = bytesToHex(chunk);

        std::ostringstream chunkCmd;
        chunkCmd << "UPLOAD_CHUNK " << m_token << " " << uploadId << " " << offset << " " << hexData;
        std::string chunkResp = sendAndReceive(chunkCmd.str());

        std::istringstream cIss(chunkResp);
        std::string cTag;
        if (!(cIss >> cTag) || cTag != "CHUNK_OK") {
            return false;
        }
        offset += currentChunkSize;
    }

    std::string finishResp = sendAndReceive("UPLOAD_FINISH " + m_token + " " + uploadId);
    std::istringstream fIss(finishResp);
    std::string fTag;
    return (fIss >> fTag) && fTag == "FILE_STORED";
}

std::optional<std::vector<uint8_t>> CloudClient::downloadFile(const std::string& filename) {
    if (m_token.empty()) return std::nullopt;

    std::string resp = sendAndReceive("DOWNLOAD " + m_token + " " + filename);
    std::istringstream iss(resp);
    std::string tag, sha256, hexData;
    size_t size = 0;

    if (!(iss >> tag >> size >> sha256 >> hexData) || tag != "FILE_DATA") {
        return std::nullopt;
    }

    auto bytes = hexToBytes(hexData);
    if (bytes.size() != size) return std::nullopt;

    std::string computedHash = computeSha256(bytes);
    if (computedHash != sha256) return std::nullopt;

    return bytes;
}

} // namespace cloud_storage
