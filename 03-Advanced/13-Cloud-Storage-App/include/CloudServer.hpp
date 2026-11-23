#pragma once

#include "SocketCompat.hpp"
#include <string>
#include <vector>
#include <map>
#include <mutex>
#include <thread>
#include <atomic>
#include <memory>

namespace cloud_storage {

struct UploadSession {
    std::string uploadId;
    std::string username;
    std::string filename;
    size_t expectedSize{0};
    std::string expectedSha256;
    std::vector<uint8_t> buffer;
    size_t receivedBytes{0};
};

struct StoredFile {
    std::string filename;
    size_t size{0};
    std::string sha256;
    std::vector<uint8_t> data;
};

class CloudServer {
public:
    explicit CloudServer(int port = 8080, const std::string& rootDir = "./cloud_vault");
    ~CloudServer();

    bool start();
    void stop();
    [[nodiscard]] bool isRunning() const noexcept;

    void registerUser(const std::string& username, const std::string& password);
    [[nodiscard]] size_t getUserCount() const;
    [[nodiscard]] size_t getStoredFileCount(const std::string& username) const;

private:
    void acceptLoop();
    void clientWorker(socket_t clientSock);

    int m_port;
    std::string m_rootDir;
    socket_t m_serverSock{INVALID_SOCK_FD};
    std::atomic<bool> m_running{false};
    std::thread m_acceptThread;

    mutable std::mutex m_mutex;
    std::map<std::string, std::string> m_users; // username -> password
    std::map<std::string, std::string> m_tokens; // token -> username
    std::map<std::string, UploadSession> m_activeUploads; // uploadId -> session
    std::map<std::string, std::map<std::string, StoredFile>> m_vault; // username -> (filename -> file)
    std::vector<std::thread> m_clientThreads;
};

} // namespace cloud_storage
