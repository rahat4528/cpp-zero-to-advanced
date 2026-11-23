#pragma once

#include <string>
#include <vector>
#include <map>
#include <memory>

namespace expense {

struct Member {
    int id{0};
    std::string name;
};

struct ExpenseItem {
    int id{0};
    std::string description;
    int payerId{0};
    double totalAmount{0.0};
    std::vector<int> participantIds; // Members sharing the expense
};

struct DebtTransfer {
    int fromMemberId{0};
    int toMemberId{0};
    double amount{0.0};
};

class ExpenseManager {
public:
    ExpenseManager() = default;

    int registerMember(const std::string& name);
    std::string getMemberName(int id) const;
    const std::vector<Member>& getMembers() const;

    int addExpense(const std::string& description, int payerId, double totalAmount,
                   const std::vector<int>& participantIds);

    const std::vector<ExpenseItem>& getExpenses() const;

    // Computes net balance for each member: positive = owed money, negative = owes money
    [[nodiscard]] std::map<int, double> calculateNetBalances() const;

    // Greedy debt simplification algorithm: reduces multi-party debts to minimum transfer count
    [[nodiscard]] std::vector<DebtTransfer> simplifyDebts() const;

    // Serialization to text stream / file
    bool saveToFile(const std::string& path) const;
    bool loadFromFile(const std::string& path);

private:
    std::vector<Member> m_members;
    std::vector<ExpenseItem> m_expenses;
    int m_nextMemberId{1};
    int m_nextExpenseId{1};
};

} // namespace expense
