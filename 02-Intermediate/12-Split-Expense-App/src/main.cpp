#include "ExpenseManager.hpp"
#include <iostream>
#include <iomanip>
#include <string>

int main(int argc, char* argv[]) {
    if (argc >= 2 && std::string(argv[1]) == "--test-run") {
        std::cout << "=================================================================\n";
        std::cout << "                 SPLIT EXPENSE APPLICATION (OOP)                 \n";
        std::cout << "=================================================================\n";
        std::cout << "[Test-Run Mode] Simulating group trip expenses and debt settlement...\n";

        expense::ExpenseManager mgr;
        int alice = mgr.registerMember("Alice");
        int bob = mgr.registerMember("Bob");
        int charlie = mgr.registerMember("Charlie");
        int diana = mgr.registerMember("Diana");

        // Alice pays $120 for dinner shared by all 4 ($30 each)
        mgr.addExpense("Dinner at Trattoria", alice, 120.0, {alice, bob, charlie, diana});
        // Bob pays $60 for taxi shared by Bob and Charlie ($30 each)
        mgr.addExpense("Airport Taxi", bob, 60.0, {bob, charlie});
        // Charlie pays $40 for snacks shared by all 4 ($10 each)
        mgr.addExpense("Road Trip Snacks", charlie, 40.0, {alice, bob, charlie, diana});

        auto balances = mgr.calculateNetBalances();
        std::cout << "Calculated Net Balances:\n";
        for (const auto& [id, bal] : balances) {
            std::cout << "  " << std::left << std::setw(10) << mgr.getMemberName(id) << ": "
                      << (bal >= 0 ? "+$" : "-$") << std::fixed << std::setprecision(2) << std::abs(bal) << "\n";
        }

        auto transfers = mgr.simplifyDebts();
        std::cout << "\nOptimized Cash Transfers (Minimal Settlement):\n";
        for (const auto& t : transfers) {
            std::cout << "  " << mgr.getMemberName(t.fromMemberId) << " pays "
                      << mgr.getMemberName(t.toMemberId) << " $"
                      << std::fixed << std::setprecision(2) << t.amount << "\n";
        }

        return 0;
    }

    std::cout << "=================================================================\n";
    std::cout << "                 SPLIT EXPENSE APPLICATION (OOP)                 \n";
    std::cout << "=================================================================\n";
    std::cout << "Run with --test-run to execute automated scenario verification.\n";
    return 0;
}
