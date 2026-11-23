#include "AtmSimulation.h"

#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>

void printHelp() {
    std::cout << "ATM Machine Simulation CLI\n"
              << "Usage:\n"
              << "  AtmSimulation --account <acc> --pin <pin> --actions \"<action_list>\"\n"
              << "  AtmSimulation --help\n"
              << "Supported actions: balance, withdraw:<amt>, deposit:<amt>, statement\n"
              << "Default Demo Account: Account=12345678, PIN=4321, Initial Balance=$2500\n"
              << "Example:\n"
              << "  AtmSimulation --account 12345678 --pin 4321 --actions \"balance;withdraw:100;deposit:50;statement\"\n";
}

int main(int argc, char* argv[]) {
    std::string accountNum;
    std::string pin;
    std::string actionString;

    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "--help" || arg == "-h") {
            printHelp();
            return 0;
        } else if (arg == "--account" && i + 1 < argc) {
            accountNum = argv[++i];
        } else if (arg == "--pin" && i + 1 < argc) {
            pin = argv[++i];
        } else if (arg == "--actions" && i + 1 < argc) {
            actionString = argv[++i];
        }
    }

    atm_simulation::AtmEngine atm;

    if (!accountNum.empty() && !pin.empty() && !actionString.empty()) {
        std::cout << "=== ATM Terminal Simulation ===\n";
        if (!atm.insertCard(accountNum)) {
            std::cerr << "Error: Card rejected or account not found.\n";
            return 1;
        }
        if (!atm.enterPin(pin)) {
            std::cerr << "Error: Invalid PIN code.\n";
            return 1;
        }

        std::cout << "Card authenticated successfully. Processing transaction batch...\n";
        std::stringstream ss(actionString);
        std::string action;

        while (std::getline(ss, action, ';')) {
            if (action == "balance") {
                auto b = atm.checkBalance();
                if (b) std::cout << ">> Current Balance: $" << std::fixed << std::setprecision(2) << *b << "\n";
            } else if (action.rfind("withdraw:", 0) == 0) {
                double amt = std::stod(action.substr(9));
                auto notes = atm.withdraw(amt);
                if (notes) {
                    std::cout << ">> Dispensed $" << amt << " ("
                              << notes->hundreds << "x$100, "
                              << notes->fifties << "x$50, "
                              << notes->twenties << "x$20)\n";
                } else {
                    std::cout << ">> Withdrawal failed for $" << amt << " (insufficient funds or limit exceeded)\n";
                }
            } else if (action.rfind("deposit:", 0) == 0) {
                double amt = std::stod(action.substr(8));
                if (atm.deposit(amt)) {
                    std::cout << ">> Deposited $" << std::fixed << std::setprecision(2) << amt << "\n";
                } else {
                    std::cout << ">> Deposit failed for $" << amt << "\n";
                }
            } else if (action == "statement") {
                std::cout << "\n>> MINI-STATEMENT:\n";
                for (const auto& rec : atm.getMiniStatement()) {
                    std::cout << "   [" << rec.timestamp << "] " << rec.description
                              << " Amount: $" << rec.amount
                              << " | Balance: $" << rec.balanceAfter << "\n";
                }
            }
        }
        atm.ejectCard();
        std::cout << "Session completed. Card ejected safely.\n";
        return 0;
    }

    printHelp();
    return 0;
}
