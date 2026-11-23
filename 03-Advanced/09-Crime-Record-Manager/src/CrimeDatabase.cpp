#include "CrimeDatabase.hpp"
#include <chrono>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <iostream>

namespace crime_manager {

static std::string currentUtcTimestamp() {
    auto now = std::chrono::system_clock::now();
    std::time_t tt = std::chrono::system_clock::to_time_t(now);
    std::tm tmVal{};
#if defined(_WIN32) || defined(_WIN64)
    gmtime_s(&tmVal, &tt);
#else
    gmtime_r(&tt, &tmVal);
#endif
    std::ostringstream oss;
    oss << std::put_time(&tmVal, "%Y-%m-%d %H:%M:%SZ");
    return oss.str();
}

CrimeDatabase::CrimeDatabase(const std::string& dbPath)
    : m_dbPath(dbPath) {}

CrimeDatabase::~CrimeDatabase() {
    close();
}

bool CrimeDatabase::open() {
    if (m_db) return true;
    int rc = sqlite3_open(m_dbPath.c_str(), &m_db);
    if (rc != SQLITE_OK) {
        close();
        return false;
    }
    return initializeSchema();
}

void CrimeDatabase::close() {
    if (m_db) {
        sqlite3_close(m_db);
        m_db = nullptr;
    }
}

bool CrimeDatabase::isOpen() const noexcept {
    return m_db != nullptr;
}

bool CrimeDatabase::initializeSchema() {
    if (!m_db) return false;

    const char* schema =
        "CREATE TABLE IF NOT EXISTS users ("
        "  id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "  username TEXT NOT NULL UNIQUE,"
        "  password_hash TEXT NOT NULL,"
        "  salt TEXT NOT NULL,"
        "  role TEXT NOT NULL"
        ");"
        "CREATE TABLE IF NOT EXISTS crime_cases ("
        "  id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "  case_number TEXT NOT NULL UNIQUE,"
        "  incident_type TEXT NOT NULL,"
        "  description TEXT NOT NULL,"
        "  status INTEGER NOT NULL,"
        "  clearance INTEGER NOT NULL,"
        "  assigned_detective TEXT,"
        "  date_filed TEXT NOT NULL"
        ");"
        "CREATE TABLE IF NOT EXISTS audit_trail ("
        "  id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "  timestamp TEXT NOT NULL,"
        "  actor TEXT NOT NULL,"
        "  action TEXT NOT NULL,"
        "  details TEXT NOT NULL,"
        "  prev_hash TEXT NOT NULL,"
        "  current_hash TEXT NOT NULL"
        ");";

    char* errMsg = nullptr;
    int rc = sqlite3_exec(m_db, schema, nullptr, nullptr, &errMsg);
    if (rc != SQLITE_OK) {
        if (errMsg) sqlite3_free(errMsg);
        return false;
    }
    return true;
}

std::string CrimeDatabase::getLastAuditHash() const {
    if (!m_db) return "GENESIS_BLOCK_00000000000000000000000000000000";

    const char* sql = "SELECT current_hash FROM audit_trail ORDER BY id DESC LIMIT 1;";
    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(m_db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        return "GENESIS_BLOCK_00000000000000000000000000000000";
    }

    std::string hash = "GENESIS_BLOCK_00000000000000000000000000000000";
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        hash = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
    }
    sqlite3_finalize(stmt);
    return hash;
}

void CrimeDatabase::logAudit(const std::string& actor, const std::string& action, const std::string& details) {
    if (!m_db) return;

    std::string prevHash = getLastAuditHash();
    std::string timestamp = currentUtcTimestamp();
    std::string toHash = prevHash + "|" + timestamp + "|" + actor + "|" + action + "|" + details;
    std::string currentHash = AuthManager::sha256Hex(toHash);

    const char* sql = "INSERT INTO audit_trail (timestamp, actor, action, details, prev_hash, current_hash) VALUES (?, ?, ?, ?, ?, ?);";
    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(m_db, sql, -1, &stmt, nullptr) != SQLITE_OK) return;

    sqlite3_bind_text(stmt, 1, timestamp.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, actor.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 3, action.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 4, details.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 5, prevHash.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 6, currentHash.c_str(), -1, SQLITE_TRANSIENT);

    sqlite3_step(stmt);
    sqlite3_finalize(stmt);
}

int CrimeDatabase::createUser(const std::string& username, const std::string& password, UserRole role) {
    if (!m_db) return -1;

    std::string salt = AuthManager::generateSalt();
    std::string hash = AuthManager::hashPassword(password, salt);
    std::string roleStr = AuthManager::roleToString(role);

    const char* sql = "INSERT INTO users (username, password_hash, salt, role) VALUES (?, ?, ?, ?);";
    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(m_db, sql, -1, &stmt, nullptr) != SQLITE_OK) return -1;

    sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, hash.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 3, salt.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 4, roleStr.c_str(), -1, SQLITE_TRANSIENT);

    int rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);

    if (rc != SQLITE_DONE) return -1;
    int newId = static_cast<int>(sqlite3_last_insert_rowid(m_db));
    logAudit("SYSTEM", "USER_CREATED", "User created: " + username + " Role: " + roleStr);
    return newId;
}

std::optional<UserSession> CrimeDatabase::authenticate(const std::string& username, const std::string& password) {
    if (!m_db) return std::nullopt;

    const char* sql = "SELECT id, password_hash, salt, role FROM users WHERE username = ?;";
    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(m_db, sql, -1, &stmt, nullptr) != SQLITE_OK) return std::nullopt;

    sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_TRANSIENT);
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        int uid = sqlite3_column_int(stmt, 0);
        std::string dbHash = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        std::string salt = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
        std::string roleStr = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3));
        sqlite3_finalize(stmt);

        std::string testHash = AuthManager::hashPassword(password, salt);
        if (testHash == dbHash) {
            UserSession session;
            session.userId = uid;
            session.username = username;
            session.role = AuthManager::stringToRole(roleStr);
            session.sessionToken = AuthManager::sha256Hex(username + salt + currentUtcTimestamp());

            logAudit(username, "LOGIN_SUCCESS", "Authenticated as " + roleStr);
            return session;
        }
    } else {
        sqlite3_finalize(stmt);
    }

    logAudit(username, "LOGIN_FAILED", "Failed password attempt");
    return std::nullopt;
}

int CrimeDatabase::createCase(const UserSession& session, const CrimeCase& c) {
    if (!m_db) return -1;
    if (!AuthManager::hasPermission(session.role, "CREATE_CASE")) {
        logAudit(session.username, "ACCESS_DENIED", "Attempted to create case without permission");
        return -1;
    }

    const char* sql =
        "INSERT INTO crime_cases (case_number, incident_type, description, status, clearance, assigned_detective, date_filed) "
        "VALUES (?, ?, ?, ?, ?, ?, ?);";
    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(m_db, sql, -1, &stmt, nullptr) != SQLITE_OK) return -1;

    sqlite3_bind_text(stmt, 1, c.caseNumber.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, c.incidentType.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 3, c.description.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(stmt, 4, static_cast<int>(c.status));
    sqlite3_bind_int(stmt, 5, static_cast<int>(c.clearance));
    sqlite3_bind_text(stmt, 6, c.assignedDetective.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 7, c.dateFiled.c_str(), -1, SQLITE_TRANSIENT);

    int rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);

    if (rc != SQLITE_DONE) return -1;
    int id = static_cast<int>(sqlite3_last_insert_rowid(m_db));
    logAudit(session.username, "CASE_CREATED", "Case #" + c.caseNumber + " (" + c.incidentType + ")");
    return id;
}

bool CrimeDatabase::updateCaseStatus(const UserSession& session, int caseId, CaseStatus newStatus, const std::string& note) {
    if (!m_db) return false;
    if (!AuthManager::hasPermission(session.role, "UPDATE_STATUS")) {
        logAudit(session.username, "ACCESS_DENIED", "Attempted to update status on case ID " + std::to_string(caseId));
        return false;
    }

    const char* sql = "UPDATE crime_cases SET status = ? WHERE id = ?;";
    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(m_db, sql, -1, &stmt, nullptr) != SQLITE_OK) return false;

    sqlite3_bind_int(stmt, 1, static_cast<int>(newStatus));
    sqlite3_bind_int(stmt, 2, caseId);

    int rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);

    if (rc == SQLITE_DONE) {
        logAudit(session.username, "CASE_STATUS_UPDATED", "Case ID " + std::to_string(caseId) + " -> status " + std::to_string(static_cast<int>(newStatus)) + " note: " + note);
        return true;
    }
    return false;
}

std::vector<CrimeCase> CrimeDatabase::getAccessibleCases(const UserSession& session) const {
    std::vector<CrimeCase> list;
    if (!m_db) return list;

    // Filter by clearance level based on role
    // Officer: Public only (clearance = 0)
    // Detective: Public and Confidential (clearance <= 1)
    // Admin: All (clearance <= 2)
    int maxClearance = 0;
    if (session.role == UserRole::Admin) maxClearance = 2;
    else if (session.role == UserRole::Detective) maxClearance = 1;

    const char* sql = "SELECT id, case_number, incident_type, description, status, clearance, assigned_detective, date_filed "
                      "FROM crime_cases WHERE clearance <= ? ORDER BY id DESC;";
    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(m_db, sql, -1, &stmt, nullptr) != SQLITE_OK) return list;

    sqlite3_bind_int(stmt, 1, maxClearance);

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        CrimeCase c;
        c.id = sqlite3_column_int(stmt, 0);
        c.caseNumber = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        c.incidentType = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
        c.description = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3));
        c.status = static_cast<CaseStatus>(sqlite3_column_int(stmt, 4));
        c.clearance = static_cast<SecurityClearance>(sqlite3_column_int(stmt, 5));
        const char* det = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 6));
        c.assignedDetective = det ? det : "";
        c.dateFiled = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 7));
        list.push_back(c);
    }
    sqlite3_finalize(stmt);
    return list;
}

std::optional<CrimeCase> CrimeDatabase::getCaseById(const UserSession& session, int caseId) const {
    auto cases = getAccessibleCases(session);
    for (const auto& c : cases) {
        if (c.id == caseId) return c;
    }
    return std::nullopt;
}

std::vector<AuditEntry> CrimeDatabase::getAuditTrail(const UserSession& session) const {
    std::vector<AuditEntry> list;
    if (!m_db) return list;
    if (!AuthManager::hasPermission(session.role, "VIEW_AUDIT")) {
        return list;
    }

    const char* sql = "SELECT id, timestamp, actor, action, details, prev_hash, current_hash FROM audit_trail ORDER BY id ASC;";
    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(m_db, sql, -1, &stmt, nullptr) != SQLITE_OK) return list;

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        AuditEntry a;
        a.id = sqlite3_column_int(stmt, 0);
        a.timestamp = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        a.actor = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
        a.action = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3));
        a.details = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 4));
        a.prevHash = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 5));
        a.currentHash = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 6));
        list.push_back(a);
    }
    sqlite3_finalize(stmt);
    return list;
}

bool CrimeDatabase::verifyAuditChainIntegrity() const {
    if (!m_db) return false;

    const char* sql = "SELECT timestamp, actor, action, details, prev_hash, current_hash FROM audit_trail ORDER BY id ASC;";
    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(m_db, sql, -1, &stmt, nullptr) != SQLITE_OK) return false;

    std::string expectedPrevHash = "GENESIS_BLOCK_00000000000000000000000000000000";

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        std::string timestamp = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
        std::string actor = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        std::string action = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
        std::string details = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3));
        std::string prevHash = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 4));
        std::string currentHash = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 5));

        if (prevHash != expectedPrevHash) {
            sqlite3_finalize(stmt);
            return false;
        }

        std::string checkToHash = prevHash + "|" + timestamp + "|" + actor + "|" + action + "|" + details;
        std::string computedHash = AuthManager::sha256Hex(checkToHash);
        if (computedHash != currentHash) {
            sqlite3_finalize(stmt);
            return false;
        }

        expectedPrevHash = currentHash;
    }
    sqlite3_finalize(stmt);
    return true;
}

} // namespace crime_manager
