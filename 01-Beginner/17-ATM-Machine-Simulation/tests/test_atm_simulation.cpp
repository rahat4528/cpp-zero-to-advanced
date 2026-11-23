#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest/doctest.h"
#include "AtmSimulation.h"

using namespace atm_simulation;

TEST_CASE("FSM Card insert and authentication flow") {
    AtmEngine atm;

    CHECK(atm.getState() == AtmState::Idle);

    // Invalid account
    CHECK(atm.insertCard("99999999") == false);
    CHECK(atm.getState() == AtmState::Idle);

    // Valid account
    CHECK(atm.insertCard("12345678") == true);
    CHECK(atm.getState() == AtmState::CardInserted);

    // Cannot check balance before authenticating
    CHECK(atm.checkBalance().has_value() == false);

    // Wrong PIN
    CHECK(atm.enterPin("0000") == false);
    CHECK(atm.getState() == AtmState::CardInserted);

    // Correct PIN
    CHECK(atm.enterPin("4321") == true);
    CHECK(atm.getState() == AtmState::Authenticated);

    auto balance = atm.checkBalance();
    REQUIRE(balance.has_value());
    CHECK(*balance == doctest::Approx(2500.0));
}

TEST_CASE("Three failed PIN attempts trigger card lockout") {
    AtmEngine atm;
    Account testAcc{"87654321", "9999", 1000.0, 0, false, 500.0, 0.0, {}};
    atm.addAccount(testAcc);

    REQUIRE(atm.insertCard("87654321") == true);

    CHECK(atm.enterPin("1111") == false); // attempt 1
    CHECK(atm.enterPin("2222") == false); // attempt 2
    CHECK(atm.enterPin("3333") == false); // attempt 3 -> Lockout!

    CHECK(atm.getState() == AtmState::LockedOut);

    // Subsequent insertion of locked card is rejected
    atm.ejectCard();
    CHECK(atm.insertCard("87654321") == false);
}

TEST_CASE("Withdrawal and Cash Note Denomination Breakdown") {
    AtmEngine atm;
    REQUIRE(atm.insertCard("12345678") == true);
    REQUIRE(atm.enterPin("4321") == true);

    // Withdraw $170 -> 1x$100, 1x$50, 1x$20
    auto notes = atm.withdraw(170.0);
    REQUIRE(notes.has_value());
    CHECK(notes->hundreds == 1);
    CHECK(notes->fifties == 1);
    CHECK(notes->twenties == 1);
    CHECK(notes->totalAmount == 170);

    // Balance should now be 2500 - 170 = 2330
    CHECK(*atm.checkBalance() == doctest::Approx(2330.0));

    // Overdraft attempt rejected
    CHECK(atm.withdraw(5000.0).has_value() == false);
}

TEST_CASE("Deposit and Mini-statement transaction history") {
    AtmEngine atm;
    REQUIRE(atm.insertCard("12345678") == true);
    REQUIRE(atm.enterPin("4321") == true);

    CHECK(atm.deposit(500.0) == true);
    CHECK(*atm.checkBalance() == doctest::Approx(3000.0));

    auto stmt = atm.getMiniStatement();
    REQUIRE(!stmt.empty());
    CHECK(stmt.back().type == TransactionType::Deposit);
    CHECK(stmt.back().amount == doctest::Approx(500.0));
    CHECK(stmt.back().balanceAfter == doctest::Approx(3000.0));
}
