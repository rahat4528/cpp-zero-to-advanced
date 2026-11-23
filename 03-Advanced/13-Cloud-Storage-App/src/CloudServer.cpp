#include "CloudServer.hpp"
#include "CloudProtocol.hpp"
#include <iostream>
#include <sstream>
#include <random>
#include <cstring>

namespace cloud_storage {

static std::string generateToken() {
    static const char chars[] = "0123456789abcdef";
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<size_t> dist(0, 15);
    std::string tok;
    tok.reserve(32);
    for (int i = 0; i < 32; ++i) tok += chars[dist(gen)];
    return tok;
}

CloudServer::CloudServer(int port, const std::string& rootDir)
    : m_port(port), m_rootDir(rootDir) {
    initCloudSockets();
    // Default system user
    registerUser("admin", "AdminSecret123");
    registerUser("developer", "DevPass2026");
}

CloudServer::~CloudServer() {
    stop();
    cleanupCloudSockets();
}

void CloudServer::registerUser(const std::string& username, const std::string& password) {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_users[username] = password;
}

size_t CloudServer::getUserCount() const {
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_users.size();
}

size_t CloudServer::getStoredFileCount(const std::string& username) const {
    std::lock_guard<std::mutex> lock(m_mutex);
    auto it = m_vault.find(username);
    if (it != m_vault.end()) {
        return it->second.size();
    }
    return 0;
}

bool CloudServer::start() {
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
        closeCloudSocket(m_serverSock);
        m_serverSock = INVALID_SOCK_FD;
        return false;
    }

    if (listen(m_serverSock, 10) == SOCK_ERROR_CODE) {
        closeCloudSocket(m_serverSock);
        m_serverSock = INVALID_SOCK_FD;
        return false;
    }

    m_running.store(true);
    m_acceptThread = std::thread(&CloudServer::acceptLoop, this);
    return true;
}

void CloudServer::stop() {
    if (!m_running.exchange(false)) return;

    if (m_serverSock != INVALID_SOCK_FD) {
        closeCloudSocket(m_serverSock);
        m_serverSock = INVALID_SOCK_FD;
    }

    if (m_acceptThread.joinable()) {
        m_acceptThread.join();
    }

    std::lock_guard<std::mutex> lock(m_mutex);
    for (auto& t : m_clientThreads) {
        if (t.joinable()) t.join();
    }
    m_clientThreads.clear();
}

bool CloudServer::isRunning() const noexcept {
    return m_running.load();
}

void CloudServer::acceptLoop() {
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

        std::lock_guard<std::mutex> lock(m_mutex);
        m_clientThreads.emplace_back(&CloudServer::clientWorker, this, clientSock);
    }
}

void CloudServer::clientWorker(socket_t clientSock) {
    char buf[4096];
    std::string streamBuf;

    auto sendResponse = [clientSock](const std::string& msg) {
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
                std::string user, pass;
                iss >> user >> pass;
                std::lock_guard<std::mutex> lock(m_mutex);
                if (m_users.count(user) && m_users[user] == pass) {
                    std::string token = generateToken();
                    m_tokens[token] = user;
                    sendResponse("TOKEN " + token);
                } else {
                    sendResponse("ERROR Invalid_Credentials");
                }
            } else if (cmd == "UPLOAD_INIT") {
                std::string token, filename, sha256;
                size_t filesize = 0;
                iss >> token >> filename >> filesize >> sha256;

                std::lock_guard<std::mutex> lock(m_mutex);
                if (!m_tokens.count(token)) {
                    sendResponse("ERROR Unauthorized");
                    continue;
                }
                std::string user = m_tokens[token];
                std::string uploadId = generateToken();

                UploadSession session;
                session.uploadId = uploadId;
                session.username = user;
                session.filename = filename;
                session.expectedSize = filesize;
                session.expectedSha256 = sha256;
                session.buffer.resize(filesize);
                m_activeUploads[uploadId] = session;

                sendResponse("UPLOAD_OK " + uploadId);
            } else if (cmd == "UPLOAD_CHUNK") {
                std::string token, uploadId, hexData;
                size_t offset = 0;
                iss >> token >> uploadId >> offset >> hexData;

                std::lock_guard<std::mutex> lock(m_mutex);
                if (!m_tokens.count(token) || !m_activeUploads.count(uploadId)) {
                    sendResponse("ERROR Invalid_Upload_Session");
                    continue;
                }
                auto& session = m_activeUploads[uploadId];
                auto chunkBytes = hexToBytes(hexData);
                if (offset + chunkBytes.size() <= session.buffer.size()) {
                    std::memcpy(session.buffer.data() + offset, chunkBytes.data(), chunkBytes.size());
                    session.receivedBytes += chunkBytes.size();
                    sendResponse("CHUNK_OK " + uploadId);
                } else {
                    sendResponse("ERROR Chunk_Overflow");
                }
            } else if (cmd == "UPLOAD_FINISH") {
                std::string token, uploadId;
                iss >> token >> uploadId;

                std::lock_guard<std::mutex> lock(m_mutex);
                if (!m_tokens.count(token) || !m_activeUploads.count(uploadId)) {
                    sendResponse("ERROR Invalid_Upload_Session");
                    continue;
                }
                auto session = m_activeUploads[uploadId];
                m_activeUploads.erase(uploadId);

                std::string computedHash = computeSha256(session.buffer);
                if (computedHash != session.expectedSha256) {
                    sendResponse("ERROR Integrity_Check_Failed");
                    continue;
                }

                StoredFile sf;
                sf.filename = session.filename;
                sf.size = session.buffer.size();
                sf.sha256 = computedHash;
                sf.data = std::move(session.buffer);
                m_vault[session.username][session.filename] = std::move(sf);

                sendResponse("FILE_STORED " + session.filename);
            } else if (cmd == "DOWNLOAD") {
                std::string token, filename;
                iss >> token >> filename;

                std::lock_guard<std::mutex> lock(m_mutex);
                if (!m_tokens.count(token)) {
                    sendResponse("ERROR Unauthorized");
                    continue;
                }
                std::string user = m_tokens[token];
                if (m_vault.count(user) && m_vault[user].count(filename)) {
                    const auto& file = m_vault[user][filename];
                    std::string hexData = bytesToHex(file.data);
                    sendResponse("FILE_DATA " + std::to_string(file.size) + " " + file.sha256 + " " + hexData);
                } else {
                    sendResponse("ERROR File_Not_Found");
                }
            } else if (cmd == "QUIT") {
                goto end_worker;
            }
        }
    }

end_worker:
    closeCloudSocket(clientSock);
}

} // namespace cloud_storage
