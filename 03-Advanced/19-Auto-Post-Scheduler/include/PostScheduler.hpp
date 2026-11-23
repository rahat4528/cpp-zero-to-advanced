#pragma once

#include "PostDatabase.hpp"
#include <map>
#include <memory>
#include <thread>
#include <atomic>
#include <mutex>
#include <condition_variable>

namespace scheduler {

class PostScheduler {
public:
    explicit PostScheduler(const std::string& dbPath = ":memory:");
    ~PostScheduler();

    void registerBackend(std::shared_ptr<IPostingBackend> backend);

    bool start();
    void stop();
    [[nodiscard]] bool isRunning() const noexcept;

    int schedule(const std::string& platform, const std::string& content, int delaySeconds = 0);
    int dispatchDueNow(int64_t currentEpochSec);

    PostDatabase& getDatabase() noexcept { return m_db; }
    [[nodiscard]] const PostDatabase& getDatabase() const noexcept { return m_db; }

private:
    void schedulerWorker();

    PostDatabase m_db;
    std::map<std::string, std::shared_ptr<IPostingBackend>> m_backends;
    std::atomic<bool> m_running{false};
    std::thread m_worker;
    std::mutex m_cvMutex;
    std::condition_variable m_cv;
};

} // namespace scheduler
