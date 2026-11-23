#include "ReportGenerator.hpp"
#include <sstream>
#include <iomanip>
#include <algorithm>

namespace budget {

std::string ReportGenerator::renderHorizontalBarChart(const std::vector<std::pair<std::string, double>>& data, int maxBarWidth) {
    if (data.empty()) return "No data available for chart.\n";

    double maxVal = 0.0;
    for (const auto& item : data) {
        if (item.second > maxVal) maxVal = item.second;
    }
    if (maxVal <= 0.0001) maxVal = 1.0;

    std::ostringstream oss;
    oss << "\n=== Spending Distribution Chart ===\n";
    for (const auto& item : data) {
        int barLength = static_cast<int>((item.second / maxVal) * maxBarWidth);
        std::string bar(static_cast<size_t>(std::max(0, barLength)), '#');

        oss << std::left << std::setw(15) << item.first
            << " | " << std::left << std::setw(maxBarWidth) << bar
            << " $" << std::fixed << std::setprecision(2) << item.second << "\n";
    }
    oss << "====================================\n";
    return oss.str();
}

std::vector<BudgetStatus> ReportGenerator::getMonthlyBudgetStatuses(const BudgetDatabase& db, const std::string& monthPrefix) {
    std::vector<BudgetStatus> statuses;
    auto categories = db.getCategories();

    for (const auto& cat : categories) {
        if (cat.type != TransactionType::Expense) continue;

        BudgetStatus status;
        status.category = cat;
        status.spent = db.getCategorySpending(cat.id, monthPrefix);
        status.remaining = cat.monthlyBudget - status.spent;
        status.percentUsed = cat.monthlyBudget > 0.0 ? (status.spent / cat.monthlyBudget) * 100.0 : 0.0;
        status.isOverBudget = status.spent > cat.monthlyBudget && cat.monthlyBudget > 0.0;
        statuses.push_back(status);
    }
    return statuses;
}

std::string ReportGenerator::generateMonthlySummary(const BudgetDatabase& db, const std::string& monthPrefix) {
    double income = db.getTotalIncome(monthPrefix);
    double expense = db.getTotalExpense(monthPrefix);
    double netSavings = income - expense;
    double savingsRate = income > 0.0 ? (netSavings / income) * 100.0 : 0.0;

    std::ostringstream oss;
    oss << "\n========================================\n";
    oss << "        FINANCIAL SUMMARY REPORT        \n";
    if (!monthPrefix.empty()) {
        oss << "             Period: " << monthPrefix << "\n";
    }
    oss << "========================================\n";
    oss << std::fixed << std::setprecision(2);
    oss << " Total Income:       $" << income << "\n";
    oss << " Total Expense:      $" << expense << "\n";
    oss << " Net Cash Flow:      $" << netSavings << "\n";
    oss << " Savings Rate:        " << savingsRate << "%\n";
    oss << "----------------------------------------\n";
    oss << " Budget Adherence:\n";

    auto statuses = getMonthlyBudgetStatuses(db, monthPrefix);
    for (const auto& s : statuses) {
        oss << "  - " << std::left << std::setw(14) << s.category.name
            << " Spent: $" << std::setw(8) << s.spent
            << " / Budget: $" << std::setw(8) << s.category.monthlyBudget
            << " (" << std::fixed << std::setprecision(1) << s.percentUsed << "%)"
            << (s.isOverBudget ? " [EXCEEDED!]" : "") << "\n";
    }

    std::vector<std::pair<std::string, double>> chartData;
    for (const auto& s : statuses) {
        if (s.spent > 0.0) {
            chartData.emplace_back(s.category.name, s.spent);
        }
    }
    oss << renderHorizontalBarChart(chartData, 30);
    return oss.str();
}

} // namespace budget
