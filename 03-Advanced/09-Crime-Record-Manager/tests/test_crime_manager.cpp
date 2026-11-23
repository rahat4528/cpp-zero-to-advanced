#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>
#include "CrimeDatabase.hpp"

using namespace crime_manager;

TEST_SUITE("Crime Record Manager and RBAC System") {
    TEST_CASE("User Authentication and Role Authorization") {
        CrimeDatabase db(":memory:");
        REQUIRE(db.open());

        int uid = db.createUser("InspectorMorse", "MorsePass99", UserRole::Detective);
        CHECK(uid > 0);

        auto session = db.authenticate("InspectorMorse", "MorsePass99");
        REQUIRE(session.has_value());
        CHECK(session->username == "InspectorMorse");
        CHECK(session->role == UserRole::Detective);

        auto failed = db.authenticate("InspectorMorse", "WrongPassword");
        CHECK_FALSE(failed.has_value());
    }

    TEST_CASE("Role Based Access Control and Clearance Filtering") {
        CrimeDatabase db(":memory:");
        REQUIRE(db.open());

        db.createUser("Chief", "ChiefPass", UserRole::Admin);
        db.createUser("Patrol", "PatrolPass", UserRole::Officer);

        auto adminSession = db.authenticate("Chief", "ChiefPass");
        auto officerSession = db.authenticate("Patrol", "PatrolPass");
        REQUIRE(adminSession.has_value());
        REQUIRE(officerSession.has_value());

        // Officer creates public case
        CrimeCase c1;
        c1.caseNumber = "PUB-001";
        c1.incidentType = "Traffic Incident";
        c1.description = "Vehicle collision at 5th and Main";
        c1.status = CaseStatus::Open;
        c1.clearance = SecurityClearance::Public;
        c1.dateFiled = "2026-11-20";
        int id1 = db.createCase(*officerSession, c1);
        CHECK(id1 > 0);

        // Admin creates top secret case
        CrimeCase c2;
        c2.caseNumber = "SEC-002";
        c2.incidentType = "Undercover Operation";
        c2.description = "Classified operational details";
        c2.status = CaseStatus::UnderInvestigation;
        c2.clearance = SecurityClearance::TopSecret;
        c2.dateFiled = "2026-11-21";
        int id2 = db.createCase(*adminSession, c2);
        CHECK(id2 > 0);

        // Officer sees only 1 case (Public)
        CHECK(db.getAccessibleCases(*officerSession).size() == 1);

        // Admin sees all 2 cases
        CHECK(db.getAccessibleCases(*adminSession).size() == 2);

        // Officer cannot update status
        CHECK_FALSE(db.updateCaseStatus(*officerSession, id1, CaseStatus::Closed, "Denied"));
    }

    TEST_CASE("Cryptographic Audit Chain Integrity") {
        CrimeDatabase db(":memory:");
        REQUIRE(db.open());

        db.createUser("AdminUser", "Admin123", UserRole::Admin);
        auto admin = db.authenticate("AdminUser", "Admin123");
        REQUIRE(admin.has_value());

        CrimeCase c;
        c.caseNumber = "AUDIT-001";
        c.incidentType = "Audit Test Case";
        c.description = "Testing blockchain hash consistency";
        c.status = CaseStatus::Open;
        c.clearance = SecurityClearance::Public;
        c.dateFiled = "2026-11-22";
        db.createCase(*admin, c);

        auto auditLog = db.getAuditTrail(*admin);
        CHECK(auditLog.size() >= 3); // USER_CREATED, LOGIN_SUCCESS, CASE_CREATED

        CHECK(db.verifyAuditChainIntegrity());
    }
}
