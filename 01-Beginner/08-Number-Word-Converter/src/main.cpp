#include "NumberWordConverter.h"

#include <iostream>
#include <string>

void printHelp() {
    std::cout << "Number to Words Converter CLI\n"
              << "Usage:\n"
              << "  NumberWordConverter --num <integer>         Convert integer to words\n"
              << "  NumberWordConverter --currency <amount>     Convert monetary value to words\n"
              << "  NumberWordConverter --help                  Show help\n"
              << "Examples:\n"
              << "  NumberWordConverter --num 1042\n"
              << "  NumberWordConverter --currency 2540.75\n";
}

int main(int argc, char* argv[]) {
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "--help" || arg == "-h") {
            printHelp();
            return 0;
        } else if (arg == "--num" && i + 1 < argc) {
            try {
                int64_t n = std::stoll(argv[++i]);
                std::cout << n << " = " << number_word_converter::numberToWords(n) << "\n";
            } catch (const std::exception& e) {
                std::cerr << "Error parsing integer: " << e.what() << "\n";
                return 1;
            }
            return 0;
        } else if (arg == "--currency" && i + 1 < argc) {
            try {
                double amt = std::stod(argv[++i]);
                std::cout << "$" << amt << " = " << number_word_converter::currencyToWords(amt) << "\n";
            } catch (const std::exception& e) {
                std::cerr << "Error parsing decimal amount: " << e.what() << "\n";
                return 1;
            }
            return 0;
        }
    }

    std::cout << "=== Number to Words Converter ===\n";
    std::cout << "Enter a number (or enter 'q' to quit): ";
    std::string line;
    if (std::cin >> line && line != "q") {
        try {
            int64_t n = std::stoll(line);
            std::cout << "Words: " << number_word_converter::numberToWords(n) << "\n";
        } catch (...) {
            std::cerr << "Invalid integer input.\n";
            return 1;
        }
    }
    return 0;
}
