#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>
#include "ExpenseManager.hpp"
#include <filesystem>

TEST_CASE("ExpenseManager - Balance Calculation and Debt Simplification") {
    expense::ExpenseManager mgr;
    int a = mgr.registerMember("Alice");
    int b = mgr.registerMember("Bob");
    int c = mgr.registerMember("Charlie");

    SUBCASE("Equal 3-Way Split") {
        // Alice pays $90 for A, B, C ($30 each)
        mgr.addExpense("Groceries", a, 90.0, {a, b, c});

        auto balances = mgr.calculateNetBalances();
        // Alice: +$60, Bob: -$30, Charlie: -$30
        CHECK(balances[a] == doctest::Approx(60.0));
        CHECK(balances[b] == doctest::Approx(-30.0));
        CHECK(balances[c] == doctest::Approx(-30.0));

        auto transfers = mgr.simplifyDebts();
        CHECK(transfers.size() == 2);
        for (const auto& t : transfers) {
            CHECK(t.toMemberId == a);
            CHECK(t.amount == doctest::Approx(30.0));
        }
    }

    SUBCASE("Multi-Party Debt Circular Settlement") {
        // A pays $60 for B
        mgr.addExpense("Ticket A->B", a, 60.0, {b});
        // B pays $60 for C
        mgr.addExpense("Ticket B->C", b, 60.0, {c});

        // Net balances: A: +$60, B: $0, C: -$60
        auto balances = mgr.calculateNetBalances();
        CHECK(balances[a] == doctest::Approx(60.0));
        CHECK(balances[b] == doctest::Approx(0.0));
        CHECK(balances[c] == doctest::Approx(-60.0));

        // Simplified transfer: C pays A $60 directly (B bypassed completely!)
        auto transfers = mgr.simplifyDebts();
        REQUIRE(transfers.size() == 1);
        CHECK(transfers[0].fromMemberId == c);
        CHECK(transfers[0].toMemberId == a);
        CHECK(transfers[0].amount == doctest::Approx(60.0));
    }
}

TEST_CASE("ExpenseManager - File Persistence Roundtrip") {
    expense::ExpenseManager original;
    int m1 = original.registerMember("X");
    int m2 = original.registerMember("Y");
    original.addExpense("Lunch", m1, 50.0, {m1, m2});

    std::string path = "temp_expense_save.txt";
    CHECK(original.saveToFile(path) == true);

    expense::ExpenseManager loaded;
    CHECK(loaded.loadFromFile(path) == true);
    CHECK(loaded.getMembers().size() == 2);
    CHECK(loaded.getExpenses().size() == 1);
    CHECK(loaded.getExpenses()[0].totalAmount == doctest::Approx(50.0));

    std::filesystem::remove(path);
}
