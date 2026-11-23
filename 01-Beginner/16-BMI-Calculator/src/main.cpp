#include "BmiCalculator.h"

#include <iomanip>
#include <iostream>
#include <string>

void printHelp() {
    std::cout << "BMI Calculator CLI\n"
              << "Usage:\n"
              << "  BmiCalculator --metric --weight <kg> --height <m>     Metric inputs\n"
              << "  BmiCalculator --imperial --weight <lbs> --height <in>  Imperial inputs\n"
              << "  BmiCalculator --help                                 Show help\n"
              << "Examples:\n"
              << "  BmiCalculator --metric --weight 70.0 --height 1.75\n"
              << "  BmiCalculator --imperial --weight 160.0 --height 68.0\n";
}

void printReport(const bmi_calculator::BmiResult& r) {
    std::cout << std::fixed << std::setprecision(1);
    std::cout << "\n============================================\n"
              << "            BODY MASS INDEX REPORT          \n"
              << "============================================\n"
              << "BMI Score       : " << r.bmiValue << " kg/m²\n"
              << "Category        : " << r.categoryName << "\n"
              << "Health Risk     : " << r.healthRisk << "\n"
              << "--------------------------------------------\n"
              << "Healthy Range   : " << r.idealWeightMinKg << " - " << r.idealWeightMaxKg << " kg ("
              << r.idealWeightMinLbs << " - " << r.idealWeightMaxLbs << " lbs)\n"
              << "============================================\n";
}

int main(int argc, char* argv[]) {
    bool isImperial = false;
    double weight = 0.0;
    double height = 0.0;
    bool hasWeight = false;
    bool hasHeight = false;

    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "--help" || arg == "-h") {
            printHelp();
            return 0;
        } else if (arg == "--metric") {
            isImperial = false;
        } else if (arg == "--imperial") {
            isImperial = true;
        } else if (arg == "--weight" && i + 1 < argc) {
            weight = std::stod(argv[++i]);
            hasWeight = true;
        } else if (arg == "--height" && i + 1 < argc) {
            height = std::stod(argv[++i]);
            hasHeight = true;
        }
    }

    if (hasWeight && hasHeight) {
        std::optional<bmi_calculator::BmiResult> res;
        if (isImperial) {
            res = bmi_calculator::BmiCalculator::calculateImperial(weight, height);
        } else {
            // If height entered in cm (> 3.0), convert to meters automatically
            if (height > 3.0) height /= 100.0;
            res = bmi_calculator::BmiCalculator::calculateMetric(weight, height);
        }

        if (res) {
            printReport(*res);
            return 0;
        } else {
            std::cerr << "Error: Invalid weight or height inputs (outside plausible human bounds).\n";
            return 1;
        }
    }

    // Interactive fallback
    std::cout << "=== BMI Calculator ===\n";
    std::cout << "Choose system (1 = Metric kg/cm, 2 = Imperial lbs/in): ";
    int choice = 1;
    if (std::cin >> choice) {
        if (choice == 2) {
            std::cout << "Enter weight in lbs: ";
            std::cin >> weight;
            std::cout << "Enter height in inches: ";
            std::cin >> height;
            auto res = bmi_calculator::BmiCalculator::calculateImperial(weight, height);
            if (res) printReport(*res);
            else std::cerr << "Invalid inputs.\n";
        } else {
            std::cout << "Enter weight in kg: ";
            std::cin >> weight;
            std::cout << "Enter height in meters (or cm): ";
            std::cin >> height;
            if (height > 3.0) height /= 100.0;
            auto res = bmi_calculator::BmiCalculator::calculateMetric(weight, height);
            if (res) printReport(*res);
            else std::cerr << "Invalid inputs.\n";
        }
    }
    return 0;
}
