#pragma once

#include <optional>
#include <string>
#include <vector>

namespace atm_simulation {

enum class AtmState {
    Idle,
    CardInserted,
    Authenticated,
    LockedOut,
    SessionEnded
};

enum class TransactionType {
    BalanceInquiry,
    Deposit,
    Withdrawal,
    PinChange
};

struct TransactionRecord {
    std::string timestamp;
    TransactionType type;
    double amount{0.0};
    double balanceAfter{0.0};
    std::string description;
};

struct Account {
    std::string accountNumber;
    std::string pin;
    double balance{0.0};
    int failedPinAttempts{0};
    bool isLocked{false};
    double dailyWithdrawalLimit{1000.0};
    double withdrawnToday{0.0};
    std::vector<TransactionRecord> history;
};

struct DispenseNotes {
    int twenties{0};
    int fifties{0};
    int hundreds{0};
    int totalAmount{0};
};

class AtmEngine {
public:
    AtmEngine();

    // Setup / demo accounts
    void addAccount(const Account& acc);
    std::optional<Account> getAccount(const std::string& accountNumber) const;

    // FSM State transitions
    bool insertCard(const std::string& accountNumber);
    bool enterPin(const std::string& pin);
    void ejectCard();

    // Transactions (allowed only when Authenticated)
    std::optional<double> checkBalance() const;
    std::optional<DispenseNotes> withdraw(double amount);
    bool deposit(double amount);
    bool changePin(const std::string& oldPin, const std::string& newPin);
    std::vector<TransactionRecord> getMiniStatement() const;

    AtmState getState() const { return m_state; }
    std::string getCurrentAccount() const { return m_currentAccount; }

    // Cash note breakdown helper (only multiples of 10 or 20 supported)
    static std::optional<DispenseNotes> calculateDispense(int amount);

private:
    AtmState m_state{AtmState::Idle};
    std::string m_currentAccount;
    std::vector<Account> m_accounts;
    Account* findActiveAccount();
    const Account* findActiveAccount() const;
};

} // namespace atm_simulation
