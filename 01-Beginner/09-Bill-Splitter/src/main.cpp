#include "BillSplitter.h"

#include <iomanip>
#include <iostream>
#include <string>

void printHelp() {
    std::cout << "Bill Splitter CLI\n"
              << "Usage:\n"
              << "  BillSplitter --total <subtotal> --people <count> [--tip <pct>] [--tax <pct>]\n"
              << "  BillSplitter --itemized \"Pizza:24:Alice,Bob;Salad:12:Bob\" [--tip <pct>] [--tax <pct>]\n"
              << "  BillSplitter --help\n"
              << "Examples:\n"
              << "  BillSplitter --total 100.00 --people 3 --tip 20 --tax 8.875\n"
              << "  BillSplitter --itemized \"Burger:15:Alice;Pasta:18:Bob;Dessert:10:Alice,Bob\" --tip 18\n";
}

int main(int argc, char* argv[]) {
    double total = 0.0;
    bool hasTotal = false;
    size_t people = 1;
    bool hasPeople = false;
    double tip = 15.0;
    double tax = 8.5;
    std::string itemizedStr;

    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "--help" || arg == "-h") {
            printHelp();
            return 0;
        } else if (arg == "--total" && i + 1 < argc) {
            total = std::stod(argv[++i]);
            hasTotal = true;
        } else if (arg == "--people" && i + 1 < argc) {
            people = std::stoul(argv[++i]);
            hasPeople = true;
        } else if (arg == "--tip" && i + 1 < argc) {
            tip = std::stod(argv[++i]);
        } else if (arg == "--tax" && i + 1 < argc) {
            tax = std::stod(argv[++i]);
        } else if (arg == "--itemized" && i + 1 < argc) {
            itemizedStr = argv[++i];
        }
    }

    if (!itemizedStr.empty()) {
        auto items = bill_splitter::parseItemizedString(itemizedStr);
        auto summaryOpt = bill_splitter::calculateItemizedSplit(items, tip, tax);
        if (!summaryOpt) {
            std::cerr << "Error: Failed to calculate itemized split. Check format and values.\n";
            return 1;
        }

        std::cout << std::fixed << std::setprecision(2);
        std::cout << "\n========================================\n"
                  << "          ITEMIZED BILL BREAKDOWN       \n"
                  << "========================================\n"
                  << "Subtotal   : $" << summaryOpt->subtotal << "\n"
                  << "Tip Rate   : " << tip << "%\n"
                  << "Tax Rate   : " << tax << "%\n"
                  << "Grand Total: $" << summaryOpt->grandTotal << "\n"
                  << "----------------------------------------\n"
                  << "INDIVIDUAL AMOUNTS OWED:\n";
        for (const auto& [person, owed] : summaryOpt->personOwes) {
            std::cout << "  " << std::left << std::setw(15) << person << ": $" << owed << "\n";
        }
        std::cout << "========================================\n";
        return 0;
    }

    if (hasTotal && hasPeople) {
        auto summaryOpt = bill_splitter::calculateEqualSplit(total, people, tip, tax);
        if (!summaryOpt) {
            std::cerr << "Error: Invalid total or people count.\n";
            return 1;
        }
        const auto& s = *summaryOpt;
        std::cout << std::fixed << std::setprecision(2);
        std::cout << "\n========================================\n"
                  << "              BILL SUMMARY              \n"
                  << "========================================\n"
                  << "Subtotal     : $" << s.subtotal << "\n"
                  << "Tax (" << s.taxRatePct << "%): $" << s.taxAmount << "\n"
                  << "Tip (" << s.tipRatePct << "%): $" << s.tipAmount << "\n"
                  << "Grand Total  : $" << s.grandTotal << "\n"
                  << "People Count : " << s.peopleCount << "\n"
                  << "----------------------------------------\n"
                  << "Base Per Person : $" << s.perPersonBase << "\n";

        if (!s.individualShares.empty()) {
            std::cout << "Exact Distribution (Penny-Exact):\n";
            for (size_t i = 0; i < s.individualShares.size(); ++i) {
                std::cout << "  Person " << (i + 1) << ": $" << s.individualShares[i] << "\n";
            }
        }
        std::cout << "========================================\n";
        return 0;
    }

    printHelp();
    return 0;
}
