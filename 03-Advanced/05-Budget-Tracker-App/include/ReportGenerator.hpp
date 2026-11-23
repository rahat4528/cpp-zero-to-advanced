#pragma once

#include "BudgetDatabase.hpp"
#include <string>
#include <vector>
#include <utility>

namespace budget {

struct BudgetStatus {
    Category category;
    double spent{0.0};
    double remaining{0.0};
    double percentUsed{0.0};
    bool isOverBudget{false};
};

class ReportGenerator {
public:
    static std::string renderHorizontalBarChart(const std::vector<std::pair<std::string, double>>& data, int maxBarWidth = 35);
    static std::vector<BudgetStatus> getMonthlyBudgetStatuses(const BudgetDatabase& db, const std::string& monthPrefix);
    static std::string generateMonthlySummary(const BudgetDatabase& db, const std::string& monthPrefix);
};

} // namespace budget
