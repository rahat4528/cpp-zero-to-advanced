#pragma once

#include <string>
#include <vector>
#include <optional>
#include <sqlite3.h>

namespace budget {

enum class TransactionType {
    Income,
    Expense
};

struct Category {
    int id{0};
    std::string name;
    TransactionType type{TransactionType::Expense};
    double monthlyBudget{0.0};
};

struct Transaction {
    int id{0};
    int categoryId{0};
    std::string categoryName;
    double amount{0.0};
    std::string date; // YYYY-MM-DD
    std::string description;
    TransactionType type{TransactionType::Expense};
};

class BudgetDatabase {
public:
    explicit BudgetDatabase(const std::string& dbPath = ":memory:");
    ~BudgetDatabase();

    bool open();
    void close();
    [[nodiscard]] bool isOpen() const;

    bool initializeSchema();

    int addCategory(const std::string& name, TransactionType type, double monthlyBudget);
    [[nodiscard]] std::vector<Category> getCategories() const;
    [[nodiscard]] std::optional<Category> getCategoryById(int id) const;

    int addTransaction(int categoryId, double amount, const std::string& date, const std::string& desc);
    [[nodiscard]] std::vector<Transaction> getTransactions(const std::string& monthPrefix = "") const;
    [[nodiscard]] double getTotalIncome(const std::string& monthPrefix = "") const;
    [[nodiscard]] double getTotalExpense(const std::string& monthPrefix = "") const;
    [[nodiscard]] double getCategorySpending(int categoryId, const std::string& monthPrefix = "") const;

private:
    std::string m_dbPath;
    sqlite3* m_db{nullptr};
};

} // namespace budget
