#include "ExpenseManager.hpp"
#include <algorithm>
#include <cmath>
#include <fstream>
#include <sstream>
#include <queue>

namespace expense {

int ExpenseManager::registerMember(const std::string& name) {
    int id = m_nextMemberId++;
    m_members.push_back({id, name});
    return id;
}

std::string ExpenseManager::getMemberName(int id) const {
    for (const auto& m : m_members) {
        if (m.id == id) return m.name;
    }
    return "Unknown";
}

const std::vector<Member>& ExpenseManager::getMembers() const {
    return m_members;
}

int ExpenseManager::addExpense(const std::string& description, int payerId, double totalAmount,
                              const std::vector<int>& participantIds) {
    if (totalAmount <= 0.0 || participantIds.empty()) return -1;

    int id = m_nextExpenseId++;
    m_expenses.push_back({id, description, payerId, totalAmount, participantIds});
    return id;
}

const std::vector<ExpenseItem>& ExpenseManager::getExpenses() const {
    return m_expenses;
}

std::map<int, double> ExpenseManager::calculateNetBalances() const {
    std::map<int, double> balances;
    for (const auto& m : m_members) {
        balances[m.id] = 0.0;
    }

    for (const auto& exp : m_expenses) {
        if (exp.participantIds.empty()) continue;
        double splitShare = exp.totalAmount / static_cast<double>(exp.participantIds.size());

        // Payer is credited the full amount
        balances[exp.payerId] += exp.totalAmount;

        // Each participant is debited their share
        for (int pId : exp.participantIds) {
            balances[pId] -= splitShare;
        }
    }

    return balances;
}

std::vector<DebtTransfer> ExpenseManager::simplifyDebts() const {
    auto balances = calculateNetBalances();

    // Priority queues: Max-heap of debtors (owes money: balance < 0) and creditors (owed money: balance > 0)
    // Store as: (amount, memberId)
    using Entry = std::pair<double, int>;
    std::priority_queue<Entry> debtors;   // top is largest debt
    std::priority_queue<Entry> creditors; // top is largest credit

    for (const auto& [id, bal] : balances) {
        if (bal < -0.005) {
            debtors.push({-bal, id}); // Store absolute amount owed
        } else if (bal > 0.005) {
            creditors.push({bal, id});
        }
    }

    std::vector<DebtTransfer> transfers;

    while (!debtors.empty() && !creditors.empty()) {
        auto debtor = debtors.top();
        debtors.pop();

        auto creditor = creditors.top();
        creditors.pop();

        double settleAmount = std::min(debtor.first, creditor.first);
        // Round to 2 decimal places
        settleAmount = std::round(settleAmount * 100.0) / 100.0;

        if (settleAmount > 0.001) {
            transfers.push_back({debtor.second, creditor.second, settleAmount});
        }

        double debtorRemaining = debtor.first - settleAmount;
        double creditorRemaining = creditor.first - settleAmount;

        if (debtorRemaining > 0.005) {
            debtors.push({debtorRemaining, debtor.second});
        }
        if (creditorRemaining > 0.005) {
            creditors.push({creditorRemaining, creditor.second});
        }
    }

    return transfers;
}

bool ExpenseManager::saveToFile(const std::string& path) const {
    std::ofstream out(path);
    if (!out.is_open()) return false;

    // Format:
    // MEMBERS <count>
    // id name
    // EXPENSES <count>
    // id payerId totalAmount description participantCount p1 p2 ...
    out << "MEMBERS " << m_members.size() << "\n";
    for (const auto& m : m_members) {
        out << m.id << " " << m.name << "\n";
    }

    out << "EXPENSES " << m_expenses.size() << "\n";
    for (const auto& e : m_expenses) {
        out << e.id << " " << e.payerId << " " << e.totalAmount << " "
            << e.participantIds.size();
        for (int pid : e.participantIds) {
            out << " " << pid;
        }
        out << " " << e.description << "\n";
    }

    return true;
}

bool ExpenseManager::loadFromFile(const std::string& path) {
    std::ifstream in(path);
    if (!in.is_open()) return false;

    m_members.clear();
    m_expenses.clear();

    std::string tag;
    size_t count = 0;

    if (in >> tag >> count && tag == "MEMBERS") {
        for (size_t i = 0; i < count; ++i) {
            Member m;
            in >> m.id >> m.name;
            m_members.push_back(m);
            if (m.id >= m_nextMemberId) m_nextMemberId = m.id + 1;
        }
    }

    if (in >> tag >> count && tag == "EXPENSES") {
        for (size_t i = 0; i < count; ++i) {
            ExpenseItem e;
            size_t pCount = 0;
            in >> e.id >> e.payerId >> e.totalAmount >> pCount;
            e.participantIds.resize(pCount);
            for (size_t p = 0; p < pCount; ++p) {
                in >> e.participantIds[p];
            }
            std::getline(in >> std::ws, e.description);
            m_expenses.push_back(e);
            if (e.id >= m_nextExpenseId) m_nextExpenseId = e.id + 1;
        }
    }

    return true;
}

} // namespace expense
