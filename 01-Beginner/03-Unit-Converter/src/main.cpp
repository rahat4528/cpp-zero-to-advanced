#include "UnitConverter.h"

#include <iomanip>
#include <iostream>
#include <string>

void printHelp() {
    std::cout << "Unit Converter CLI\n"
              << "Usage:\n"
              << "  UnitConverter --cat <category> --from <unit> --to <unit> --val <value>\n"
              << "  UnitConverter --list\n"
              << "  UnitConverter --help\n"
              << "Examples:\n"
              << "  UnitConverter --cat length --from km --to mi --val 10\n"
              << "  UnitConverter --cat temp --from c --to f --val 100\n";
}

int main(int argc, char* argv[]) {
    std::string catStr;
    std::string fromUnit;
    std::string toUnit;
    double val = 0.0;
    bool hasVal = false;

    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "--help" || arg == "-h") {
            printHelp();
            return 0;
        } else if (arg == "--list") {
            std::cout << "Categories and supported units:\n"
                      << "  Length      : m, km, cm, mm, mi, yd, ft, in\n"
                      << "  Mass        : kg, g, mg, lb, oz, ton\n"
                      << "  Temperature : c (Celsius), f (Fahrenheit), k (Kelvin)\n"
                      << "  Speed       : mps, kmh, mph, knot\n";
            return 0;
        } else if (arg == "--cat" && i + 1 < argc) {
            catStr = argv[++i];
        } else if (arg == "--from" && i + 1 < argc) {
            fromUnit = argv[++i];
        } else if (arg == "--to" && i + 1 < argc) {
            toUnit = argv[++i];
        } else if (arg == "--val" && i + 1 < argc) {
            val = std::stod(argv[++i]);
            hasVal = true;
        }
    }

    if (!catStr.empty() && !fromUnit.empty() && !toUnit.empty() && hasVal) {
        auto catOpt = unit_converter::parseCategory(catStr);
        if (!catOpt) {
            std::cerr << "Error: Unknown category '" << catStr << "'. Use --list to see supported categories.\n";
            return 1;
        }
        auto result = unit_converter::convert(*catOpt, val, fromUnit, toUnit);
        if (!result) {
            std::cerr << "Error: Conversion failed. Check that units match category and values do not violate physical limits (e.g. negative lengths/temperatures below absolute zero).\n";
            return 1;
        }
        std::cout << std::fixed << std::setprecision(4)
                  << val << " " << fromUnit << " = "
                  << result->convertedValue << " " << toUnit << "\n";
        return 0;
    }

    // Interactive fallback
    std::cout << "=== Unit Converter ===\n";
    std::cout << "Available categories: length, mass, temperature, speed\n";
    std::cout << "Enter category: ";
    if (!(std::cin >> catStr)) return 0;

    auto catOpt = unit_converter::parseCategory(catStr);
    if (!catOpt) {
        std::cerr << "Invalid category.\n";
        return 1;
    }

    std::cout << "Enter source unit: ";
    std::cin >> fromUnit;
    std::cout << "Enter target unit: ";
    std::cin >> toUnit;
    std::cout << "Enter value: ";
    if (!(std::cin >> val)) {
        std::cerr << "Invalid numeric value.\n";
        return 1;
    }

    auto result = unit_converter::convert(*catOpt, val, fromUnit, toUnit);
    if (!result) {
        std::cerr << "Conversion failed. Please verify unit codes and constraints.\n";
        return 1;
    }

    std::cout << "\nResult: " << std::fixed << std::setprecision(4)
              << val << " " << fromUnit << " = "
              << result->convertedValue << " " << toUnit << "\n";
    return 0;
}
