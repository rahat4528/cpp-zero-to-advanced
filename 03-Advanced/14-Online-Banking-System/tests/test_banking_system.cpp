#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>
#include "BankingDatabase.hpp"
#include "BankingServer.hpp"
#include "BankingClient.hpp"
#include <thread>
#include <chrono>

using namespace banking;

TEST_SUITE("Online Banking System with SQLite ACID Transactions") {
    TEST_CASE("Account Creation and Authentication") {
        BankingDatabase db(":memory:");
        REQUIRE(db.open());

        int id = db.createAccount("ACC-001", "Charlie", "9999", 100000);
        CHECK(id > 0);

        auto auth = db.authenticate("ACC-001", "9999");
        REQUIRE(auth.has_value());
        CHECK(auth->username == "Charlie");
        CHECK(auth->balanceCents == 100000);

        CHECK_FALSE(db.authenticate("ACC-001", "0000").has_value());
    }

    TEST_CASE("ACID Fund Transfer and Rollback Integrity") {
        BankingDatabase db(":memory:");
        REQUIRE(db.open());

        db.createAccount("A1", "UserA", "1111", 50000); // 500.00
        db.createAccount("B1", "UserB", "2222", 10000); // 100.00

        std::string ref, err;
        // Valid transfer of 200.00
        bool ok = db.executeTransfer("A1", "B1", 20000, ref, err);
        CHECK(ok);
        CHECK_FALSE(ref.empty());
        CHECK(db.getBalance("A1") == 30000);
        CHECK(db.getBalance("B1") == 30000);

        // Insufficient funds transfer of 400.00 (balance only 300.00)
        bool over = db.executeTransfer("A1", "B1", 40000, ref, err);
        CHECK_FALSE(over);
        CHECK(err.find("Insufficient funds") != std::string::npos);
        // Verify ACID rollback kept balances strictly intact
        CHECK(db.getBalance("A1") == 30000);
        CHECK(db.getBalance("B1") == 30000);

        // Negative amount rejected
        CHECK_FALSE(db.executeTransfer("A1", "B1", -500, ref, err));

        // Ledger history records 1 successful transaction
        auto historyA = db.getHistory("A1");
        CHECK(historyA.size() == 1);
        CHECK(historyA[0].status == "SETTLED");
    }

    TEST_CASE("Networked Client-Server Transfer Protocol") {
        int testPort = 9470;
        BankingServer server(testPort);
        server.getDatabase().createAccount("SRV-A", "Sender", "1234", 80000);
        server.getDatabase().createAccount("SRV-B", "Receiver", "5678", 20000);
        REQUIRE(server.start());

        std::this_thread::sleep_for(std::chrono::milliseconds(50));

        BankingClient client;
        REQUIRE(client.connectToServer("127.0.0.1", testPort));
        REQUIRE(client.login("SRV-A", "1234"));

        auto bal = client.queryBalance();
        REQUIRE(bal.has_value());
        CHECK(*bal == 80000);

        std::string ref, err;
        CHECK(client.transferFunds("SRV-B", 30000, ref, err));

        auto newBal = client.queryBalance();
        REQUIRE(newBal.has_value());
        CHECK(*newBal == 50000);

        client.disconnect();
        server.stop();
    }
}
