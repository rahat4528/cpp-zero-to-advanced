#include "TemperatureConverter.h"

#include <algorithm>
#include <cctype>
#include <cmath>
#include <iomanip>
#include <sstream>

namespace temperature_converter {

static std::string toLower(std::string s) {
    std::transform(s.begin(), s.end(), s.begin(), [](unsigned char c) {
        return static_cast<char>(std::tolower(c));
    });
    return s;
}

std::optional<Scale> TemperatureConverter::parseScale(const std::string& scaleStr) {
    std::string l = toLower(scaleStr);
    if (l == "c" || l == "celsius") return Scale::Celsius;
    if (l == "f" || l == "fahrenheit") return Scale::Fahrenheit;
    if (l == "k" || l == "kelvin") return Scale::Kelvin;
    if (l == "r" || l == "rankine") return Scale::Rankine;
    return std::nullopt;
}

std::string TemperatureConverter::scaleToString(Scale scale) {
    switch (scale) {
        case Scale::Celsius: return "Celsius";
        case Scale::Fahrenheit: return "Fahrenheit";
        case Scale::Kelvin: return "Kelvin";
        case Scale::Rankine: return "Rankine";
    }
    return "Unknown";
}

std::optional<TemperatureReading> TemperatureConverter::convert(double value, Scale fromScale) {
    if (std::isnan(value)) return std::nullopt;

    // Convert to Celsius baseline first
    double c = 0.0;
    switch (fromScale) {
        case Scale::Celsius:
            c = value;
            break;
        case Scale::Fahrenheit:
            c = (value - 32.0) * (5.0 / 9.0);
            break;
        case Scale::Kelvin:
            if (value < 0.0) return std::nullopt; // Below 0 K
            c = value - 273.15;
            break;
        case Scale::Rankine:
            if (value < 0.0) return std::nullopt; // Below 0 R
            c = (value - 491.67) * (5.0 / 9.0);
            break;
    }

    // Absolute zero barrier: -273.15 °C
    if (c < -273.15) {
        return std::nullopt;
    }

    TemperatureReading tr;
    tr.celsius = c;
    tr.fahrenheit = (c * (9.0 / 5.0)) + 32.0;
    tr.kelvin = c + 273.15;
    tr.rankine = (c + 273.15) * (9.0 / 5.0);

    if (c <= 0.0) {
        tr.stateDescription = "Freezing (Sub-zero)";
    } else if (c < 15.0) {
        tr.stateDescription = "Cool";
    } else if (c <= 25.0) {
        tr.stateDescription = "Comfortable Room Temp";
    } else if (c < 100.0) {
        tr.stateDescription = "Warm / Hot";
    } else {
        tr.stateDescription = "Boiling Point or Higher";
    }

    return tr;
}

std::string TemperatureConverter::renderThermometerGauge(double celsius) {
    // Map -20°C to 120°C across a 20-segment gauge
    const int gaugeWidth = 20;
    const double minTemp = -20.0;
    const double maxTemp = 120.0;

    double clamped = std::clamp(celsius, minTemp, maxTemp);
    double ratio = (clamped - minTemp) / (maxTemp - minTemp);
    int filledSegments = static_cast<int>(std::round(ratio * gaugeWidth));

    std::ostringstream oss;
    oss << "[";
    for (int i = 0; i < gaugeWidth; ++i) {
        if (i < filledSegments) {
            oss << "=";
        } else if (i == filledSegments) {
            oss << "|";
        } else {
            oss << ".";
        }
    }
    oss << "] " << std::fixed << std::setprecision(1) << celsius << " °C";
    return oss.str();
}

} // namespace temperature_converter
