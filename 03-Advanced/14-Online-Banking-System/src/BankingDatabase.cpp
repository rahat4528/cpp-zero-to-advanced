#include "BankingDatabase.hpp"
#include <chrono>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <random>
#include <iostream>

namespace banking {

static std::string currentTimestamp() {
    auto now = std::chrono::system_clock::now();
    std::time_t tt = std::chrono::system_clock::to_time_t(now);
    std::tm tmVal{};
#if defined(_WIN32) || defined(_WIN64)
    gmtime_s(&tmVal, &tt);
#else
    gmtime_r(&tt, &tmVal);
#endif
    std::ostringstream oss;
    oss << std::put_time(&tmVal, "%Y-%m-%d %H:%M:%SZ");
    return oss.str();
}

static std::string generateTxRef() {
    static const char chars[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<size_t> dist(0, sizeof(chars) - 2);
    std::string ref = "TX-";
    for (int i = 0; i < 8; ++i) ref += chars[dist(gen)];
    return ref;
}

BankingDatabase::BankingDatabase(const std::string& dbPath)
    : m_dbPath(dbPath) {}

BankingDatabase::~BankingDatabase() {
    close();
}

bool BankingDatabase::open() {
    if (m_db) return true;
    int rc = sqlite3_open(m_dbPath.c_str(), &m_db);
    if (rc != SQLITE_OK) {
        close();
        return false;
    }
    return initializeSchema();
}

void BankingDatabase::close() {
    if (m_db) {
        sqlite3_close(m_db);
        m_db = nullptr;
    }
}

bool BankingDatabase::isOpen() const noexcept {
    return m_db != nullptr;
}

bool BankingDatabase::initializeSchema() {
    if (!m_db) return false;

    const char* schema =
        "CREATE TABLE IF NOT EXISTS accounts ("
        "  id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "  account_number TEXT NOT NULL UNIQUE,"
        "  username TEXT NOT NULL,"
        "  pin TEXT NOT NULL,"
        "  balance_cents INTEGER NOT NULL,"
        "  is_active INTEGER NOT NULL DEFAULT 1"
        ");"
        "CREATE TABLE IF NOT EXISTS transactions ("
        "  id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "  tx_ref TEXT NOT NULL UNIQUE,"
        "  from_account TEXT NOT NULL,"
        "  to_account TEXT NOT NULL,"
        "  amount_cents INTEGER NOT NULL,"
        "  timestamp TEXT NOT NULL,"
        "  status TEXT NOT NULL"
        ");";

    char* errMsg = nullptr;
    int rc = sqlite3_exec(m_db, schema, nullptr, nullptr, &errMsg);
    if (rc != SQLITE_OK) {
        if (errMsg) sqlite3_free(errMsg);
        return false;
    }
    return true;
}

int BankingDatabase::createAccount(const std::string& accountNum, const std::string& username, const std::string& pin, int64_t initialBalanceCents) {
    if (!m_db) return -1;

    const char* sql = "INSERT INTO accounts (account_number, username, pin, balance_cents, is_active) VALUES (?, ?, ?, ?, 1);";
    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(m_db, sql, -1, &stmt, nullptr) != SQLITE_OK) return -1;

    sqlite3_bind_text(stmt, 1, accountNum.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, username.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 3, pin.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int64(stmt, 4, initialBalanceCents);

    int rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);

    if (rc != SQLITE_DONE) return -1;
    return static_cast<int>(sqlite3_last_insert_rowid(m_db));
}

std::optional<Account> BankingDatabase::authenticate(const std::string& accountNum, const std::string& pin) {
    if (!m_db) return std::nullopt;

    const char* sql = "SELECT id, account_number, username, balance_cents, is_active FROM accounts WHERE account_number = ? AND pin = ?;";
    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(m_db, sql, -1, &stmt, nullptr) != SQLITE_OK) return std::nullopt;

    sqlite3_bind_text(stmt, 1, accountNum.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, pin.c_str(), -1, SQLITE_TRANSIENT);

    if (sqlite3_step(stmt) == SQLITE_ROW) {
        Account acc;
        acc.id = sqlite3_column_int(stmt, 0);
        acc.accountNumber = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        acc.username = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
        acc.balanceCents = sqlite3_column_int64(stmt, 3);
        acc.isActive = (sqlite3_column_int(stmt, 4) == 1);
        sqlite3_finalize(stmt);
        return acc;
    }
    sqlite3_finalize(stmt);
    return std::nullopt;
}

std::optional<int64_t> BankingDatabase::getBalance(const std::string& accountNum) const {
    if (!m_db) return std::nullopt;

    const char* sql = "SELECT balance_cents FROM accounts WHERE account_number = ?;";
    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(m_db, sql, -1, &stmt, nullptr) != SQLITE_OK) return std::nullopt;

    sqlite3_bind_text(stmt, 1, accountNum.c_str(), -1, SQLITE_TRANSIENT);
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        int64_t balance = sqlite3_column_int64(stmt, 0);
        sqlite3_finalize(stmt);
        return balance;
    }
    sqlite3_finalize(stmt);
    return std::nullopt;
}

bool BankingDatabase::executeTransfer(const std::string& fromAccount, const std::string& toAccount, int64_t amountCents, std::string& outRef, std::string& outErr) {
    if (!m_db) {
        outErr = "Database not open";
        return false;
    }
    if (amountCents <= 0) {
        outErr = "Transfer amount must be strictly positive";
        return false;
    }
    if (fromAccount == toAccount) {
        outErr = "Cannot transfer to same account";
        return false;
    }

    // Begin ACID immediate transaction
    char* errMsg = nullptr;
    if (sqlite3_exec(m_db, "BEGIN IMMEDIATE TRANSACTION;", nullptr, nullptr, &errMsg) != SQLITE_OK) {
        outErr = errMsg ? errMsg : "Failed to begin transaction";
        if (errMsg) sqlite3_free(errMsg);
        return false;
    }

    auto rollback = [this]() {
        sqlite3_exec(m_db, "ROLLBACK;", nullptr, nullptr, nullptr);
    };

    // 1. Verify and lock source account
    auto fromBalOpt = getBalance(fromAccount);
    if (!fromBalOpt.has_value()) {
        rollback();
        outErr = "Source account not found";
        return false;
    }
    if (*fromBalOpt < amountCents) {
        rollback();
        outErr = "Insufficient funds (Balance: " + std::to_string(*fromBalOpt) + " cents, Requested: " + std::to_string(amountCents) + " cents)";
        return false;
    }

    // 2. Verify destination account
    auto toBalOpt = getBalance(toAccount);
    if (!toBalOpt.has_value()) {
        rollback();
        outErr = "Destination account not found";
        return false;
    }

    // 3. Debit source account
    const char* debitSql = "UPDATE accounts SET balance_cents = balance_cents - ? WHERE account_number = ?;";
    sqlite3_stmt* debitStmt = nullptr;
    if (sqlite3_prepare_v2(m_db, debitSql, -1, &debitStmt, nullptr) != SQLITE_OK) {
        rollback();
        outErr = "Failed to prepare debit";
        return false;
    }
    sqlite3_bind_int64(debitStmt, 1, amountCents);
    sqlite3_bind_text(debitStmt, 2, fromAccount.c_str(), -1, SQLITE_TRANSIENT);
    if (sqlite3_step(debitStmt) != SQLITE_DONE) {
        sqlite3_finalize(debitStmt);
        rollback();
        outErr = "Failed to debit source account";
        return false;
    }
    sqlite3_finalize(debitStmt);

    // 4. Credit destination account
    const char* creditSql = "UPDATE accounts SET balance_cents = balance_cents + ? WHERE account_number = ?;";
    sqlite3_stmt* creditStmt = nullptr;
    if (sqlite3_prepare_v2(m_db, creditSql, -1, &creditStmt, nullptr) != SQLITE_OK) {
        rollback();
        outErr = "Failed to prepare credit";
        return false;
    }
    sqlite3_bind_int64(creditStmt, 1, amountCents);
    sqlite3_bind_text(creditStmt, 2, toAccount.c_str(), -1, SQLITE_TRANSIENT);
    if (sqlite3_step(creditStmt) != SQLITE_DONE) {
        sqlite3_finalize(creditStmt);
        rollback();
        outErr = "Failed to credit destination account";
        return false;
    }
    sqlite3_finalize(creditStmt);

    // 5. Insert ledger record
    std::string ref = generateTxRef();
    std::string ts = currentTimestamp();
    const char* logSql = "INSERT INTO transactions (tx_ref, from_account, to_account, amount_cents, timestamp, status) VALUES (?, ?, ?, ?, ?, 'SETTLED');";
    sqlite3_stmt* logStmt = nullptr;
    if (sqlite3_prepare_v2(m_db, logSql, -1, &logStmt, nullptr) != SQLITE_OK) {
        rollback();
        outErr = "Failed to prepare transaction log";
        return false;
    }
    sqlite3_bind_text(logStmt, 1, ref.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(logStmt, 2, fromAccount.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(logStmt, 3, toAccount.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int64(logStmt, 4, amountCents);
    sqlite3_bind_text(logStmt, 5, ts.c_str(), -1, SQLITE_TRANSIENT);

    if (sqlite3_step(logStmt) != SQLITE_DONE) {
        sqlite3_finalize(logStmt);
        rollback();
        outErr = "Failed to record transaction ledger";
        return false;
    }
    sqlite3_finalize(logStmt);

    // 6. Commit transaction
    if (sqlite3_exec(m_db, "COMMIT;", nullptr, nullptr, nullptr) != SQLITE_OK) {
        rollback();
        outErr = "Commit failed";
        return false;
    }

    outRef = ref;
    return true;
}

std::vector<BankTransaction> BankingDatabase::getHistory(const std::string& accountNum) const {
    std::vector<BankTransaction> list;
    if (!m_db) return list;

    const char* sql =
        "SELECT id, tx_ref, from_account, to_account, amount_cents, timestamp, status "
        "FROM transactions WHERE from_account = ? OR to_account = ? ORDER BY id DESC;";
    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(m_db, sql, -1, &stmt, nullptr) != SQLITE_OK) return list;

    sqlite3_bind_text(stmt, 1, accountNum.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, accountNum.c_str(), -1, SQLITE_TRANSIENT);

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        BankTransaction tx;
        tx.id = sqlite3_column_int(stmt, 0);
        tx.ref = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        tx.fromAccount = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
        tx.toAccount = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3));
        tx.amountCents = sqlite3_column_int64(stmt, 4);
        tx.timestamp = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 5));
        tx.status = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 6));
        list.push_back(tx);
    }
    sqlite3_finalize(stmt);
    return list;
}

} // namespace banking
