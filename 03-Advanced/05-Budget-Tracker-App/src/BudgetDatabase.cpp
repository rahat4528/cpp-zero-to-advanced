#include "BudgetDatabase.hpp"
#include <stdexcept>
#include <iostream>

namespace budget {

BudgetDatabase::BudgetDatabase(const std::string& dbPath)
    : m_dbPath(dbPath) {}

BudgetDatabase::~BudgetDatabase() {
    close();
}

bool BudgetDatabase::open() {
    if (m_db) return true;
    int rc = sqlite3_open(m_dbPath.c_str(), &m_db);
    if (rc != SQLITE_OK) {
        close();
        return false;
    }
    return initializeSchema();
}

void BudgetDatabase::close() {
    if (m_db) {
        sqlite3_close(m_db);
        m_db = nullptr;
    }
}

bool BudgetDatabase::isOpen() const {
    return m_db != nullptr;
}

bool BudgetDatabase::initializeSchema() {
    if (!m_db) return false;

    const char* schema =
        "CREATE TABLE IF NOT EXISTS categories ("
        "  id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "  name TEXT NOT NULL UNIQUE,"
        "  type INTEGER NOT NULL," // 0 = Income, 1 = Expense
        "  monthly_budget REAL DEFAULT 0.0"
        ");"
        "CREATE TABLE IF NOT EXISTS transactions ("
        "  id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "  category_id INTEGER NOT NULL,"
        "  amount REAL NOT NULL,"
        "  date TEXT NOT NULL,"
        "  description TEXT,"
        "  FOREIGN KEY (category_id) REFERENCES categories(id)"
        ");";

    char* errMsg = nullptr;
    int rc = sqlite3_exec(m_db, schema, nullptr, nullptr, &errMsg);
    if (rc != SQLITE_OK) {
        if (errMsg) {
            sqlite3_free(errMsg);
        }
        return false;
    }
    return true;
}

int BudgetDatabase::addCategory(const std::string& name, TransactionType type, double monthlyBudget) {
    if (!m_db) return -1;

    const char* sql = "INSERT INTO categories (name, type, monthly_budget) VALUES (?, ?, ?);";
    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(m_db, sql, -1, &stmt, nullptr) != SQLITE_OK) return -1;

    sqlite3_bind_text(stmt, 1, name.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(stmt, 2, static_cast<int>(type));
    sqlite3_bind_double(stmt, 3, monthlyBudget);

    int rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);

    if (rc != SQLITE_DONE) return -1;
    return static_cast<int>(sqlite3_last_insert_rowid(m_db));
}

std::vector<Category> BudgetDatabase::getCategories() const {
    std::vector<Category> list;
    if (!m_db) return list;

    const char* sql = "SELECT id, name, type, monthly_budget FROM categories ORDER BY id;";
    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(m_db, sql, -1, &stmt, nullptr) != SQLITE_OK) return list;

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        Category c;
        c.id = sqlite3_column_int(stmt, 0);
        c.name = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        c.type = static_cast<TransactionType>(sqlite3_column_int(stmt, 2));
        c.monthlyBudget = sqlite3_column_double(stmt, 3);
        list.push_back(c);
    }
    sqlite3_finalize(stmt);
    return list;
}

std::optional<Category> BudgetDatabase::getCategoryById(int id) const {
    if (!m_db) return std::nullopt;

    const char* sql = "SELECT id, name, type, monthly_budget FROM categories WHERE id = ?;";
    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(m_db, sql, -1, &stmt, nullptr) != SQLITE_OK) return std::nullopt;

    sqlite3_bind_int(stmt, 1, id);
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        Category c;
        c.id = sqlite3_column_int(stmt, 0);
        c.name = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        c.type = static_cast<TransactionType>(sqlite3_column_int(stmt, 2));
        c.monthlyBudget = sqlite3_column_double(stmt, 3);
        sqlite3_finalize(stmt);
        return c;
    }
    sqlite3_finalize(stmt);
    return std::nullopt;
}

int BudgetDatabase::addTransaction(int categoryId, double amount, const std::string& date, const std::string& desc) {
    if (!m_db) return -1;

    const char* sql = "INSERT INTO transactions (category_id, amount, date, description) VALUES (?, ?, ?, ?);";
    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(m_db, sql, -1, &stmt, nullptr) != SQLITE_OK) return -1;

    sqlite3_bind_int(stmt, 1, categoryId);
    sqlite3_bind_double(stmt, 2, amount);
    sqlite3_bind_text(stmt, 3, date.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 4, desc.c_str(), -1, SQLITE_TRANSIENT);

    int rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);

    if (rc != SQLITE_DONE) return -1;
    return static_cast<int>(sqlite3_last_insert_rowid(m_db));
}

std::vector<Transaction> BudgetDatabase::getTransactions(const std::string& monthPrefix) const {
    std::vector<Transaction> list;
    if (!m_db) return list;

    std::string sql =
        "SELECT t.id, t.category_id, c.name, t.amount, t.date, t.description, c.type "
        "FROM transactions t JOIN categories c ON t.category_id = c.id ";
    if (!monthPrefix.empty()) {
        sql += "WHERE t.date LIKE ? ";
    }
    sql += "ORDER BY t.date DESC, t.id DESC;";

    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(m_db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) return list;

    if (!monthPrefix.empty()) {
        std::string pattern = monthPrefix + "%";
        sqlite3_bind_text(stmt, 1, pattern.c_str(), -1, SQLITE_TRANSIENT);
    }

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        Transaction t;
        t.id = sqlite3_column_int(stmt, 0);
        t.categoryId = sqlite3_column_int(stmt, 1);
        t.categoryName = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
        t.amount = sqlite3_column_double(stmt, 3);
        t.date = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 4));
        const char* desc = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 5));
        t.description = desc ? desc : "";
        t.type = static_cast<TransactionType>(sqlite3_column_int(stmt, 6));
        list.push_back(t);
    }
    sqlite3_finalize(stmt);
    return list;
}

double BudgetDatabase::getTotalIncome(const std::string& monthPrefix) const {
    if (!m_db) return 0.0;
    std::string sql =
        "SELECT COALESCE(SUM(t.amount), 0.0) FROM transactions t "
        "JOIN categories c ON t.category_id = c.id WHERE c.type = 0 ";
    if (!monthPrefix.empty()) {
        sql += "AND t.date LIKE ?;";
    }

    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(m_db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) return 0.0;

    if (!monthPrefix.empty()) {
        std::string pattern = monthPrefix + "%";
        sqlite3_bind_text(stmt, 1, pattern.c_str(), -1, SQLITE_TRANSIENT);
    }

    double total = 0.0;
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        total = sqlite3_column_double(stmt, 0);
    }
    sqlite3_finalize(stmt);
    return total;
}

double BudgetDatabase::getTotalExpense(const std::string& monthPrefix) const {
    if (!m_db) return 0.0;
    std::string sql =
        "SELECT COALESCE(SUM(t.amount), 0.0) FROM transactions t "
        "JOIN categories c ON t.category_id = c.id WHERE c.type = 1 ";
    if (!monthPrefix.empty()) {
        sql += "AND t.date LIKE ?;";
    }

    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(m_db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) return 0.0;

    if (!monthPrefix.empty()) {
        std::string pattern = monthPrefix + "%";
        sqlite3_bind_text(stmt, 1, pattern.c_str(), -1, SQLITE_TRANSIENT);
    }

    double total = 0.0;
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        total = sqlite3_column_double(stmt, 0);
    }
    sqlite3_finalize(stmt);
    return total;
}

double BudgetDatabase::getCategorySpending(int categoryId, const std::string& monthPrefix) const {
    if (!m_db) return 0.0;
    std::string sql = "SELECT COALESCE(SUM(amount), 0.0) FROM transactions WHERE category_id = ? ";
    if (!monthPrefix.empty()) {
        sql += "AND date LIKE ?;";
    }

    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(m_db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) return 0.0;

    sqlite3_bind_int(stmt, 1, categoryId);
    if (!monthPrefix.empty()) {
        std::string pattern = monthPrefix + "%";
        sqlite3_bind_text(stmt, 2, pattern.c_str(), -1, SQLITE_TRANSIENT);
    }

    double total = 0.0;
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        total = sqlite3_column_double(stmt, 0);
    }
    sqlite3_finalize(stmt);
    return total;
}

} // namespace budget
