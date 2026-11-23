#include "PostDatabase.hpp"
#include <iostream>

namespace scheduler {

PostDatabase::PostDatabase(const std::string& dbPath)
    : m_dbPath(dbPath) {}

PostDatabase::~PostDatabase() {
    close();
}

bool PostDatabase::open() {
    if (m_db) return true;
    int rc = sqlite3_open(m_dbPath.c_str(), &m_db);
    if (rc != SQLITE_OK) {
        close();
        return false;
    }
    return initializeSchema();
}

void PostDatabase::close() {
    if (m_db) {
        sqlite3_close(m_db);
        m_db = nullptr;
    }
}

bool PostDatabase::isOpen() const noexcept {
    return m_db != nullptr;
}

bool PostDatabase::initializeSchema() {
    if (!m_db) return false;

    const char* schema =
        "CREATE TABLE IF NOT EXISTS scheduled_posts ("
        "  id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "  platform TEXT NOT NULL,"
        "  content TEXT NOT NULL,"
        "  scheduled_epoch INTEGER NOT NULL,"
        "  status TEXT NOT NULL DEFAULT 'PENDING',"
        "  retry_count INTEGER NOT NULL DEFAULT 0,"
        "  max_retries INTEGER NOT NULL DEFAULT 3,"
        "  response_data TEXT"
        ");";

    char* errMsg = nullptr;
    int rc = sqlite3_exec(m_db, schema, nullptr, nullptr, &errMsg);
    if (rc != SQLITE_OK) {
        if (errMsg) sqlite3_free(errMsg);
        return false;
    }
    return true;
}

int PostDatabase::schedulePost(const std::string& platform, const std::string& content, int64_t scheduledEpochSec, int maxRetries) {
    if (!m_db) return -1;

    const char* sql = "INSERT INTO scheduled_posts (platform, content, scheduled_epoch, status, retry_count, max_retries) VALUES (?, ?, ?, 'PENDING', 0, ?);";
    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(m_db, sql, -1, &stmt, nullptr) != SQLITE_OK) return -1;

    sqlite3_bind_text(stmt, 1, platform.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, content.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int64(stmt, 3, scheduledEpochSec);
    sqlite3_bind_int(stmt, 4, maxRetries);

    int rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);

    if (rc != SQLITE_DONE) return -1;
    return static_cast<int>(sqlite3_last_insert_rowid(m_db));
}

std::vector<ScheduledPost> PostDatabase::getPendingDuePosts(int64_t currentEpochSec) const {
    std::vector<ScheduledPost> list;
    if (!m_db) return list;

    const char* sql = "SELECT id, platform, content, scheduled_epoch, status, retry_count, max_retries, response_data "
                      "FROM scheduled_posts WHERE status = 'PENDING' AND scheduled_epoch <= ? ORDER BY scheduled_epoch ASC;";
    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(m_db, sql, -1, &stmt, nullptr) != SQLITE_OK) return list;

    sqlite3_bind_int64(stmt, 1, currentEpochSec);

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        ScheduledPost p;
        p.id = sqlite3_column_int(stmt, 0);
        p.platform = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        p.content = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
        p.scheduledEpochSec = sqlite3_column_int64(stmt, 3);
        p.status = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 4));
        p.retryCount = sqlite3_column_int(stmt, 5);
        p.maxRetries = sqlite3_column_int(stmt, 6);
        const char* resp = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 7));
        p.responseData = resp ? resp : "";
        list.push_back(p);
    }
    sqlite3_finalize(stmt);
    return list;
}

bool PostDatabase::markPostStatus(int postId, const std::string& status, const std::string& response) {
    if (!m_db) return false;

    const char* sql = "UPDATE scheduled_posts SET status = ?, response_data = ? WHERE id = ?;";
    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(m_db, sql, -1, &stmt, nullptr) != SQLITE_OK) return false;

    sqlite3_bind_text(stmt, 1, status.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, response.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(stmt, 3, postId);

    int rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    return rc == SQLITE_DONE;
}

bool PostDatabase::incrementRetry(int postId, int64_t nextAttemptEpochSec, const std::string& errorMsg) {
    if (!m_db) return false;

    const char* sql =
        "UPDATE scheduled_posts SET retry_count = retry_count + 1, scheduled_epoch = ?, "
        "status = CASE WHEN retry_count + 1 >= max_retries THEN 'FAILED' ELSE 'PENDING' END, "
        "response_data = ? WHERE id = ?;";
    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(m_db, sql, -1, &stmt, nullptr) != SQLITE_OK) return false;

    sqlite3_bind_int64(stmt, 1, nextAttemptEpochSec);
    sqlite3_bind_text(stmt, 2, errorMsg.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(stmt, 3, postId);

    int rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    return rc == SQLITE_DONE;
}

std::vector<ScheduledPost> PostDatabase::getAllPosts() const {
    std::vector<ScheduledPost> list;
    if (!m_db) return list;

    const char* sql = "SELECT id, platform, content, scheduled_epoch, status, retry_count, max_retries, response_data FROM scheduled_posts ORDER BY id ASC;";
    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(m_db, sql, -1, &stmt, nullptr) != SQLITE_OK) return list;

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        ScheduledPost p;
        p.id = sqlite3_column_int(stmt, 0);
        p.platform = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        p.content = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
        p.scheduledEpochSec = sqlite3_column_int64(stmt, 3);
        p.status = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 4));
        p.retryCount = sqlite3_column_int(stmt, 5);
        p.maxRetries = sqlite3_column_int(stmt, 6);
        const char* resp = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 7));
        p.responseData = resp ? resp : "";
        list.push_back(p);
    }
    sqlite3_finalize(stmt);
    return list;
}

std::optional<ScheduledPost> PostDatabase::getPostById(int postId) const {
    if (!m_db) return std::nullopt;

    const char* sql = "SELECT id, platform, content, scheduled_epoch, status, retry_count, max_retries, response_data FROM scheduled_posts WHERE id = ?;";
    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(m_db, sql, -1, &stmt, nullptr) != SQLITE_OK) return std::nullopt;

    sqlite3_bind_int(stmt, 1, postId);
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        ScheduledPost p;
        p.id = sqlite3_column_int(stmt, 0);
        p.platform = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        p.content = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
        p.scheduledEpochSec = sqlite3_column_int64(stmt, 3);
        p.status = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 4));
        p.retryCount = sqlite3_column_int(stmt, 5);
        p.maxRetries = sqlite3_column_int(stmt, 6);
        const char* resp = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 7));
        p.responseData = resp ? resp : "";
        sqlite3_finalize(stmt);
        return p;
    }
    sqlite3_finalize(stmt);
    return std::nullopt;
}

} // namespace scheduler
