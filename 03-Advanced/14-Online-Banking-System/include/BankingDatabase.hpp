#pragma once

#include <string>
#include <vector>
#include <optional>
#include <cstdint>
#include <sqlite3.h>

namespace banking {

struct Account {
    int id{0};
    std::string accountNumber;
    std::string username;
    int64_t balanceCents{0};
    bool isActive{true};
};

struct BankTransaction {
    int id{0};
    std::string ref;
    std::string fromAccount;
    std::string toAccount;
    int64_t amountCents{0};
    std::string timestamp;
    std::string status;
};

class BankingDatabase {
public:
    explicit BankingDatabase(const std::string& dbPath = ":memory:");
    ~BankingDatabase();

    bool open();
    void close();
    [[nodiscard]] bool isOpen() const noexcept;

    bool initializeSchema();

    int createAccount(const std::string& accountNum, const std::string& username, const std::string& pin, int64_t initialBalanceCents);
    [[nodiscard]] std::optional<Account> authenticate(const std::string& accountNum, const std::string& pin);

    [[nodiscard]] std::optional<int64_t> getBalance(const std::string& accountNum) const;
    bool executeTransfer(const std::string& fromAccount, const std::string& toAccount, int64_t amountCents, std::string& outRef, std::string& outErr);
    [[nodiscard]] std::vector<BankTransaction> getHistory(const std::string& accountNum) const;

private:
    std::string m_dbPath;
    sqlite3* m_db{nullptr};
};

} // namespace banking
