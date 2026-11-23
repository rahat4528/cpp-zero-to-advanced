#pragma once

#include "IPostingBackend.hpp"
#include <sqlite3.h>
#include <string>
#include <vector>
#include <optional>

namespace scheduler {

class PostDatabase {
public:
    explicit PostDatabase(const std::string& dbPath = ":memory:");
    ~PostDatabase();

    bool open();
    void close();
    [[nodiscard]] bool isOpen() const noexcept;

    bool initializeSchema();

    int schedulePost(const std::string& platform, const std::string& content, int64_t scheduledEpochSec, int maxRetries = 3);
    [[nodiscard]] std::vector<ScheduledPost> getPendingDuePosts(int64_t currentEpochSec) const;
    bool markPostStatus(int postId, const std::string& status, const std::string& response);
    bool incrementRetry(int postId, int64_t nextAttemptEpochSec, const std::string& errorMsg);
    [[nodiscard]] std::vector<ScheduledPost> getAllPosts() const;
    [[nodiscard]] std::optional<ScheduledPost> getPostById(int postId) const;

private:
    std::string m_dbPath;
    sqlite3* m_db{nullptr};
};

} // namespace scheduler
