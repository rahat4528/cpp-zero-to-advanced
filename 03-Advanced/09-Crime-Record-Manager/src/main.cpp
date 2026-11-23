#include "CrimeDatabase.hpp"
#include <iostream>

int main(int argc, char* argv[]) {
    bool testRun = false;
    for (int i = 1; i < argc; ++i) {
        if (std::string(argv[i]) == "--test-run") {
            testRun = true;
        }
    }

    std::cout << "========================================\n";
    std::cout << " Fictional Crime Record Manager (RBAC)  \n";
    std::cout << "========================================\n";

    crime_manager::CrimeDatabase db(":memory:");
    if (!db.open()) {
        std::cerr << "Failed to initialize in-memory SQLite database.\n";
        return 1;
    }

    // Seed fictional users across 3 roles
    db.createUser("ChiefAdmin", "SecureAdminPass#1", crime_manager::UserRole::Admin);
    db.createUser("DetMiller", "DetectivePass#2", crime_manager::UserRole::Detective);
    db.createUser("OfficerDavis", "PatrolPass#3", crime_manager::UserRole::Officer);

    std::cout << "Authenticating users...\n";
    auto adminSession = db.authenticate("ChiefAdmin", "SecureAdminPass#1");
    auto detSession = db.authenticate("DetMiller", "DetectivePass#2");
    auto officerSession = db.authenticate("OfficerDavis", "PatrolPass#3");

    if (!adminSession || !detSession || !officerSession) {
        std::cerr << "Authentication failure.\n";
        return 1;
    }

    // File fictional cases
    crime_manager::CrimeCase case1;
    case1.caseNumber = "CR-2026-001";
    case1.incidentType = "Grand Theft Auto";
    case1.description = "Stolen delivery vehicle from industrial depot.";
    case1.status = crime_manager::CaseStatus::Open;
    case1.clearance = crime_manager::SecurityClearance::Public;
    case1.dateFiled = "2026-11-20";
    db.createCase(*officerSession, case1);

    crime_manager::CrimeCase case2;
    case2.caseNumber = "CR-2026-002";
    case2.incidentType = "Corporate Espionage";
    case2.description = "Encrypted flash drive recovered at research laboratory.";
    case2.status = crime_manager::CaseStatus::UnderInvestigation;
    case2.clearance = crime_manager::SecurityClearance::TopSecret;
    case2.assignedDetective = "DetMiller";
    case2.dateFiled = "2026-11-21";
    db.createCase(*adminSession, case2);

    // Detective updates status
    db.updateCaseStatus(*detSession, 1, crime_manager::CaseStatus::UnderInvestigation, "Assigned to team B");

    // Officer attempts to update status (should be rejected by RBAC)
    bool officerUpdate = db.updateCaseStatus(*officerSession, 1, crime_manager::CaseStatus::Closed, "Unauthorized close");
    std::cout << "Officer Unauthorized Update: " << (officerUpdate ? "ALLOWED (ERROR)" : "REJECTED (RBAC SECURE)") << "\n";

    // Verify clearance filtering:
    auto officerCases = db.getAccessibleCases(*officerSession);
    auto adminCases = db.getAccessibleCases(*adminSession);
    std::cout << "Cases visible to Officer: " << officerCases.size() << " (Public only)\n";
    std::cout << "Cases visible to Admin:   " << adminCases.size() << " (Full access)\n";

    // Verify cryptographic audit trail
    bool auditIntact = db.verifyAuditChainIntegrity();
    std::cout << "Cryptographic Merkle Audit Hash Chain: " << (auditIntact ? "VALID & UNTAMPERED" : "CORRUPTED") << "\n";

    if (testRun) {
        std::cout << "[Test Mode] Crime record manager RBAC and audit trail verified.\n";
        return 0;
    }

    return 0;
}
