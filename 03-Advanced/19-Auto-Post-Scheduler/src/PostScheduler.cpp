#include "PostScheduler.hpp"
#include <chrono>
#include <iostream>

namespace scheduler {

static int64_t getNowEpochSec() {
    return std::chrono::duration_cast<std::chrono::seconds>(
        std::chrono::system_clock::now().time_since_epoch()
    ).count();
}

PostScheduler::PostScheduler(const std::string& dbPath)
    : m_db(dbPath) {
    m_db.open();
    // Register default mock backends
    registerBackend(std::make_shared<MockPostingBackend>("Twitter"));
    registerBackend(std::make_shared<MockPostingBackend>("LinkedIn"));
    registerBackend(std::make_shared<MockPostingBackend>("Discord"));
}

PostScheduler::~PostScheduler() {
    stop();
}

void PostScheduler::registerBackend(std::shared_ptr<IPostingBackend> backend) {
    if (backend) {
        m_backends[backend->getPlatformName()] = backend;
    }
}

bool PostScheduler::start() {
    if (m_running.load()) return true;
    m_running.store(true);
    m_worker = std::thread(&PostScheduler::schedulerWorker, this);
    return true;
}

void PostScheduler::stop() {
    if (!m_running.exchange(false)) return;
    m_cv.notify_all();
    if (m_worker.joinable()) {
        m_worker.join();
    }
}

bool PostScheduler::isRunning() const noexcept {
    return m_running.load();
}

int PostScheduler::schedule(const std::string& platform, const std::string& content, int delaySeconds) {
    int64_t epoch = getNowEpochSec() + delaySeconds;
    int id = m_db.schedulePost(platform, content, epoch);
    m_cv.notify_all();
    return id;
}

int PostScheduler::dispatchDueNow(int64_t currentEpochSec) {
    auto duePosts = m_db.getPendingDuePosts(currentEpochSec);
    int dispatched = 0;

    for (const auto& post : duePosts) {
        auto it = m_backends.find(post.platform);
        if (it != m_backends.end() && it->second) {
            std::string outResp;
            if (it->second->dispatchPost(post, outResp)) {
                m_db.markPostStatus(post.id, "PUBLISHED", outResp);
                dispatched++;
            } else {
                int64_t retryEpoch = currentEpochSec + (post.retryCount + 1) * 30; // backoff
                m_db.incrementRetry(post.id, retryEpoch, "Dispatch failed, backoff queued");
            }
        } else {
            m_db.markPostStatus(post.id, "FAILED", "No backend registered for platform: " + post.platform);
        }
    }
    return dispatched;
}

void PostScheduler::schedulerWorker() {
    while (m_running.load()) {
        int64_t now = getNowEpochSec();
        dispatchDueNow(now);

        std::unique_lock<std::mutex> lock(m_cvMutex);
        m_cv.wait_for(lock, std::chrono::milliseconds(500), [this]() {
            return !m_running.load();
        });
    }
}

} // namespace scheduler
