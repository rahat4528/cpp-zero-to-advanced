#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>
#include "BudgetDatabase.hpp"
#include "ReportGenerator.hpp"

using namespace budget;

TEST_SUITE("Advanced Budget Tracker System") {
    TEST_CASE("Category and Transaction Persistence in SQLite") {
        BudgetDatabase db(":memory:");
        REQUIRE(db.open());

        int incId = db.addCategory("Consulting", TransactionType::Income, 0.0);
        int expId = db.addCategory("Groceries", TransactionType::Expense, 500.0);
        CHECK(incId > 0);
        CHECK(expId > 0);

        auto categories = db.getCategories();
        CHECK(categories.size() == 2);

        int t1 = db.addTransaction(incId, 2500.0, "2026-11-01", "Invoice #101");
        int t2 = db.addTransaction(expId, 120.0, "2026-11-03", "Supermarket trip");
        int t3 = db.addTransaction(expId, 80.0, "2026-11-10", "Farmers market");
        CHECK(t1 > 0);
        CHECK(t2 > 0);
        CHECK(t3 > 0);

        CHECK(db.getTotalIncome("2026-11") == doctest::Approx(2500.0));
        CHECK(db.getTotalExpense("2026-11") == doctest::Approx(200.0));
        CHECK(db.getCategorySpending(expId, "2026-11") == doctest::Approx(200.0));
    }

    TEST_CASE("Budget Status and Alert Computations") {
        BudgetDatabase db(":memory:");
        REQUIRE(db.open());

        int diningId = db.addCategory("Dining", TransactionType::Expense, 100.0);
        db.addTransaction(diningId, 80.0, "2026-11-05", "Lunch with client");

        auto statuses = ReportGenerator::getMonthlyBudgetStatuses(db, "2026-11");
        REQUIRE(statuses.size() == 1);
        CHECK(statuses[0].spent == doctest::Approx(80.0));
        CHECK(statuses[0].remaining == doctest::Approx(20.0));
        CHECK(statuses[0].percentUsed == doctest::Approx(80.0));
        CHECK_FALSE(statuses[0].isOverBudget);

        // Exceed budget
        db.addTransaction(diningId, 35.0, "2026-11-12", "Dinner party");
        statuses = ReportGenerator::getMonthlyBudgetStatuses(db, "2026-11");
        CHECK(statuses[0].spent == doctest::Approx(115.0));
        CHECK(statuses[0].remaining == doctest::Approx(-15.0));
        CHECK(statuses[0].isOverBudget);
    }

    TEST_CASE("ASCII Chart and Monthly Summary Generation") {
        BudgetDatabase db(":memory:");
        REQUIRE(db.open());

        int techId = db.addCategory("Tech Gear", TransactionType::Expense, 500.0);
        db.addTransaction(techId, 250.0, "2026-11-01", "Mechanical Keyboard");

        std::string summary = ReportGenerator::generateMonthlySummary(db, "2026-11");
        CHECK(summary.find("FINANCIAL SUMMARY REPORT") != std::string::npos);
        CHECK(summary.find("Tech Gear") != std::string::npos);
        CHECK(summary.find("Total Expense:      $250.00") != std::string::npos);

        std::vector<std::pair<std::string, double>> chartData = {
            {"Rent", 1000.0},
            {"Food", 500.0}
        };
        std::string chart = ReportGenerator::renderHorizontalBarChart(chartData, 20);
        CHECK(chart.find("Spending Distribution Chart") != std::string::npos);
        CHECK(chart.find("Rent") != std::string::npos);
    }
}
