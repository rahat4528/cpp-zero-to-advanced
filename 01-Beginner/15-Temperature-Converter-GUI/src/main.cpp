#include "TemperatureConverter.h"

#include <iomanip>
#include <iostream>
#include <string>

void printHelp() {
    std::cout << "Temperature Converter GUI / TUI\n"
              << "Usage:\n"
              << "  TemperatureConverter --val <temp> --scale <c|f|k|r>\n"
              << "  TemperatureConverter --help\n"
              << "Examples:\n"
              << "  TemperatureConverter --val 100 --scale c\n"
              << "  TemperatureConverter --val 32 --scale f\n";
}

void printReading(const temperature_converter::TemperatureReading& tr) {
    std::cout << std::fixed << std::setprecision(2);
    std::cout << "\n====================================================\n"
              << "           TEMPERATURE CONVERSION REPORT            \n"
              << "====================================================\n"
              << "Visual Gauge : " << temperature_converter::TemperatureConverter::renderThermometerGauge(tr.celsius) << "\n"
              << "Status/Phase : " << tr.stateDescription << "\n"
              << "----------------------------------------------------\n"
              << "Celsius      : " << tr.celsius << " °C\n"
              << "Fahrenheit   : " << tr.fahrenheit << " °F\n"
              << "Kelvin       : " << tr.kelvin << " K\n"
              << "Rankine      : " << tr.rankine << " °R\n"
              << "====================================================\n";
}

int main(int argc, char* argv[]) {
    double val = 0.0;
    bool hasVal = false;
    std::string scaleStr;

    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "--help" || arg == "-h") {
            printHelp();
            return 0;
        } else if (arg == "--val" && i + 1 < argc) {
            val = std::stod(argv[++i]);
            hasVal = true;
        } else if (arg == "--scale" && i + 1 < argc) {
            scaleStr = argv[++i];
        }
    }

    if (hasVal && !scaleStr.empty()) {
        auto scaleOpt = temperature_converter::TemperatureConverter::parseScale(scaleStr);
        if (!scaleOpt) {
            std::cerr << "Error: Unknown scale '" << scaleStr << "'. Use c, f, k, or r.\n";
            return 1;
        }
        auto readingOpt = temperature_converter::TemperatureConverter::convert(val, *scaleOpt);
        if (!readingOpt) {
            std::cerr << "Error: Value is below absolute zero (-273.15 °C / 0 K / 0 °R).\n";
            return 1;
        }
        printReading(*readingOpt);
        return 0;
    }

    std::cout << "=== Temperature Converter GUI / TUI ===\n";
    std::cout << "Enter temperature value: ";
    if (std::cin >> val) {
        std::cout << "Enter scale (C, F, K, R): ";
        std::cin >> scaleStr;
        auto scaleOpt = temperature_converter::TemperatureConverter::parseScale(scaleStr);
        if (scaleOpt) {
            auto reading = temperature_converter::TemperatureConverter::convert(val, *scaleOpt);
            if (reading) {
                printReading(*reading);
            } else {
                std::cerr << "Temperature is below absolute zero.\n";
            }
        }
    }
    return 0;
}
