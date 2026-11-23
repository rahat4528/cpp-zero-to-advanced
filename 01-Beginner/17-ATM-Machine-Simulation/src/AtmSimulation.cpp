#include "AtmSimulation.h"

#include <chrono>
#include <ctime>
#include <iomanip>
#include <sstream>

namespace atm_simulation {

static std::string getTimestamp() {
    auto now = std::chrono::system_clock::now();
    std::time_t tt = std::chrono::system_clock::to_time_t(now);
    std::tm ti{};
#if defined(_WIN32)
    localtime_s(&ti, &tt);
#else
    localtime_r(&tt, &ti);
#endif
    std::ostringstream oss;
    oss << std::setfill('0')
        << std::setw(2) << ti.tm_hour << ":"
        << std::setw(2) << ti.tm_min << ":"
        << std::setw(2) << ti.tm_sec;
    return oss.str();
}

AtmEngine::AtmEngine() {
    // Seed sample educational testing account
    Account demoAcc;
    demoAcc.accountNumber = "12345678";
    demoAcc.pin = "4321";
    demoAcc.balance = 2500.0;
    demoAcc.dailyWithdrawalLimit = 1000.0;
    m_accounts.push_back(demoAcc);
}

void AtmEngine::addAccount(const Account& acc) {
    m_accounts.push_back(acc);
}

std::optional<Account> AtmEngine::getAccount(const std::string& accountNumber) const {
    for (const auto& a : m_accounts) {
        if (a.accountNumber == accountNumber) return a;
    }
    return std::nullopt;
}

Account* AtmEngine::findActiveAccount() {
    for (auto& a : m_accounts) {
        if (a.accountNumber == m_currentAccount) return &a;
    }
    return nullptr;
}

const Account* AtmEngine::findActiveAccount() const {
    for (const auto& a : m_accounts) {
        if (a.accountNumber == m_currentAccount) return &a;
    }
    return nullptr;
}

bool AtmEngine::insertCard(const std::string& accountNumber) {
    if (m_state != AtmState::Idle) return false;

    for (const auto& a : m_accounts) {
        if (a.accountNumber == accountNumber) {
            if (a.isLocked) {
                m_state = AtmState::LockedOut;
                return false;
            }
            m_currentAccount = accountNumber;
            m_state = AtmState::CardInserted;
            return true;
        }
    }
    return false; // Unknown card
}

bool AtmEngine::enterPin(const std::string& pin) {
    if (m_state != AtmState::CardInserted) return false;

    Account* acc = findActiveAccount();
    if (!acc) return false;

    if (acc->pin == pin) {
        acc->failedPinAttempts = 0;
        m_state = AtmState::Authenticated;
        return true;
    }

    acc->failedPinAttempts++;
    if (acc->failedPinAttempts >= 3) {
        acc->isLocked = true;
        m_state = AtmState::LockedOut;
    }
    return false;
}

void AtmEngine::ejectCard() {
    m_currentAccount.clear();
    m_state = AtmState::Idle;
}

std::optional<double> AtmEngine::checkBalance() const {
    if (m_state != AtmState::Authenticated) return std::nullopt;
    const Account* acc = findActiveAccount();
    if (!acc) return std::nullopt;
    return acc->balance;
}

std::optional<DispenseNotes> AtmEngine::calculateDispense(int amount) {
    if (amount <= 0 || amount % 10 != 0) return std::nullopt;

    DispenseNotes notes;
    int remaining = amount;

    notes.hundreds = remaining / 100;
    remaining %= 100;

    notes.fifties = remaining / 50;
    remaining %= 50;

    notes.twenties = remaining / 20;
    remaining %= 20;

    if (remaining != 0) {
        // Adjust for combinations like 60, 80
        if (notes.fifties > 0 && remaining == 10) {
            notes.fifties -= 1;
            remaining += 50; // 60
            notes.twenties += remaining / 20;
            remaining %= 20;
        }
    }

    if (remaining != 0) {
        return std::nullopt; // Cannot dispense with standard denominations
    }

    notes.totalAmount = amount;
    return notes;
}

std::optional<DispenseNotes> AtmEngine::withdraw(double amount) {
    if (m_state != AtmState::Authenticated) return std::nullopt;
    Account* acc = findActiveAccount();
    if (!acc) return std::nullopt;

    if (amount <= 0.0 || amount > acc->balance) return std::nullopt;
    if (acc->withdrawnToday + amount > acc->dailyWithdrawalLimit) return std::nullopt;

    int intAmt = static_cast<int>(amount);
    if (static_cast<double>(intAmt) != amount) return std::nullopt; // Cash dispense must be integer currency

    auto notesOpt = calculateDispense(intAmt);
    if (!notesOpt) return std::nullopt;

    acc->balance -= amount;
    acc->withdrawnToday += amount;

    TransactionRecord rec;
    rec.timestamp = getTimestamp();
    rec.type = TransactionType::Withdrawal;
    rec.amount = amount;
    rec.balanceAfter = acc->balance;
    rec.description = "ATM Cash Withdrawal";
    acc->history.push_back(rec);

    return notesOpt;
}

bool AtmEngine::deposit(double amount) {
    if (m_state != AtmState::Authenticated) return false;
    Account* acc = findActiveAccount();
    if (!acc || amount <= 0.0) return false;

    acc->balance += amount;

    TransactionRecord rec;
    rec.timestamp = getTimestamp();
    rec.type = TransactionType::Deposit;
    rec.amount = amount;
    rec.balanceAfter = acc->balance;
    rec.description = "ATM Cash/Check Deposit";
    acc->history.push_back(rec);

    return true;
}

bool AtmEngine::changePin(const std::string& oldPin, const std::string& newPin) {
    if (m_state != AtmState::Authenticated) return false;
    Account* acc = findActiveAccount();
    if (!acc) return false;

    if (acc->pin != oldPin || newPin.length() < 4) return false;
    acc->pin = newPin;

    TransactionRecord rec;
    rec.timestamp = getTimestamp();
    rec.type = TransactionType::PinChange;
    rec.amount = 0.0;
    rec.balanceAfter = acc->balance;
    rec.description = "Customer PIN Change";
    acc->history.push_back(rec);

    return true;
}

std::vector<TransactionRecord> AtmEngine::getMiniStatement() const {
    if (m_state != AtmState::Authenticated) return {};
    const Account* acc = findActiveAccount();
    if (!acc) return {};
    return acc->history;
}

} // namespace atm_simulation
