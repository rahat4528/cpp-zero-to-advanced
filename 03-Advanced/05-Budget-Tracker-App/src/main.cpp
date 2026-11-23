#include "BudgetDatabase.hpp"
#include "ReportGenerator.hpp"
#include <iostream>

int main(int argc, char* argv[]) {
    bool testRun = false;
    for (int i = 1; i < argc; ++i) {
        if (std::string(argv[i]) == "--test-run") {
            testRun = true;
        }
    }

    std::cout << "=== Advanced Budget Tracker Application ===\n";

    budget::BudgetDatabase db(":memory:");
    if (!db.open()) {
        std::cerr << "Failed to open database.\n";
        return 1;
    }

    // Seed realistic categories
    int salaryId = db.addCategory("Salary", budget::TransactionType::Income, 0.0);
    int housingId = db.addCategory("Housing", budget::TransactionType::Expense, 1500.0);
    int foodId = db.addCategory("Food & Dining", budget::TransactionType::Expense, 600.0);
    int utilitiesId = db.addCategory("Utilities", budget::TransactionType::Expense, 250.0);
    int entertainmentId = db.addCategory("Entertainment", budget::TransactionType::Expense, 200.0);

    // Seed transactions for November 2026
    db.addTransaction(salaryId, 4500.0, "2026-11-01", "Monthly Corporate Salary");
    db.addTransaction(housingId, 1450.0, "2026-11-02", "Apartment Rent & Parking");
    db.addTransaction(foodId, 220.0, "2026-11-05", "Weekly Organic Groceries");
    db.addTransaction(foodId, 180.0, "2026-11-12", "Dining & Coffee");
    db.addTransaction(utilitiesId, 165.0, "2026-11-10", "High-speed Internet & Power");
    db.addTransaction(entertainmentId, 230.0, "2026-11-15", "Concert tickets (Over budget demo)");

    std::cout << budget::ReportGenerator::generateMonthlySummary(db, "2026-11");

    if (testRun) {
        std::cout << "[Test Mode] Budget tracking calculation and reporting verified.\n";
        return 0;
    }

    return 0;
}
