#pragma once

#include "AuthManager.hpp"
#include <sqlite3.h>
#include <string>
#include <vector>
#include <optional>

namespace crime_manager {

enum class CaseStatus {
    Open = 0,
    UnderInvestigation = 1,
    Closed = 2,
    ColdCase = 3
};

enum class SecurityClearance {
    Public = 0,
    Confidential = 1,
    TopSecret = 2
};

struct CrimeCase {
    int id{0};
    std::string caseNumber;
    std::string incidentType;
    std::string description;
    CaseStatus status{CaseStatus::Open};
    SecurityClearance clearance{SecurityClearance::Public};
    std::string assignedDetective;
    std::string dateFiled;
};

struct AuditEntry {
    int id{0};
    std::string timestamp;
    std::string actor;
    std::string action;
    std::string details;
    std::string prevHash;
    std::string currentHash;
};

class CrimeDatabase {
public:
    explicit CrimeDatabase(const std::string& dbPath = ":memory:");
    ~CrimeDatabase();

    bool open();
    void close();
    [[nodiscard]] bool isOpen() const noexcept;

    bool initializeSchema();

    int createUser(const std::string& username, const std::string& password, UserRole role);
    [[nodiscard]] std::optional<UserSession> authenticate(const std::string& username, const std::string& password);

    int createCase(const UserSession& session, const CrimeCase& c);
    bool updateCaseStatus(const UserSession& session, int caseId, CaseStatus newStatus, const std::string& note);
    [[nodiscard]] std::vector<CrimeCase> getAccessibleCases(const UserSession& session) const;
    [[nodiscard]] std::optional<CrimeCase> getCaseById(const UserSession& session, int caseId) const;

    [[nodiscard]] std::vector<AuditEntry> getAuditTrail(const UserSession& session) const;
    [[nodiscard]] bool verifyAuditChainIntegrity() const;

private:
    void logAudit(const std::string& actor, const std::string& action, const std::string& details);
    [[nodiscard]] std::string getLastAuditHash() const;

    std::string m_dbPath;
    sqlite3* m_db{nullptr};
};

} // namespace crime_manager
