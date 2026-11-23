#include "UnitConverter.h"

#include <algorithm>
#include <cmath>
#include <cctype>
#include <unordered_map>

namespace unit_converter {

static std::string toLower(std::string s) {
    std::transform(s.begin(), s.end(), s.begin(), [](unsigned char c) {
        return static_cast<char>(std::tolower(c));
    });
    return s;
}

std::optional<Category> parseCategory(const std::string& catStr) {
    std::string lower = toLower(catStr);
    if (lower == "length" || lower == "len" || lower == "distance") return Category::Length;
    if (lower == "mass" || lower == "weight") return Category::Mass;
    if (lower == "temperature" || lower == "temp") return Category::Temperature;
    if (lower == "speed" || lower == "velocity") return Category::Speed;
    return std::nullopt;
}

std::vector<std::string> getSupportedUnits(Category category) {
    switch (category) {
        case Category::Length:
            return {"m", "km", "cm", "mm", "mi", "yd", "ft", "in"};
        case Category::Mass:
            return {"kg", "g", "mg", "lb", "oz", "ton"};
        case Category::Temperature:
            return {"c", "f", "k"};
        case Category::Speed:
            return {"mps", "kmh", "mph", "knot"};
    }
    return {};
}

// Length: base unit is meters (m)
static const std::unordered_map<std::string, double> kLengthToMeters = {
    {"m", 1.0},
    {"meter", 1.0},
    {"meters", 1.0},
    {"km", 1000.0},
    {"kilometer", 1000.0},
    {"kilometers", 1000.0},
    {"cm", 0.01},
    {"centimeter", 0.01},
    {"mm", 0.001},
    {"millimeter", 0.001},
    {"mi", 1609.344},
    {"mile", 1609.344},
    {"miles", 1609.344},
    {"yd", 0.9144},
    {"yard", 0.9144},
    {"ft", 0.3048},
    {"foot", 0.3048},
    {"feet", 0.3048},
    {"in", 0.0254},
    {"inch", 0.0254},
    {"inches", 0.0254}
};

std::optional<double> convertLength(double value, const std::string& from, const std::string& to) {
    if (value < 0.0) return std::nullopt; // Length cannot be negative
    std::string fromLower = toLower(from);
    std::string toLowerStr = toLower(to);

    auto itFrom = kLengthToMeters.find(fromLower);
    auto itTo = kLengthToMeters.find(toLowerStr);
    if (itFrom == kLengthToMeters.end() || itTo == kLengthToMeters.end()) {
        return std::nullopt;
    }
    double inMeters = value * itFrom->second;
    return inMeters / itTo->second;
}

// Mass: base unit is grams (g)
static const std::unordered_map<std::string, double> kMassToGrams = {
    {"g", 1.0},
    {"gram", 1.0},
    {"grams", 1.0},
    {"kg", 1000.0},
    {"kilogram", 1000.0},
    {"kilograms", 1000.0},
    {"mg", 0.001},
    {"milligram", 0.001},
    {"lb", 453.59237},
    {"pound", 453.59237},
    {"pounds", 453.59237},
    {"oz", 28.349523},
    {"ounce", 28.349523},
    {"ounces", 28.349523},
    {"ton", 1000000.0},
    {"tonne", 1000000.0}
};

std::optional<double> convertMass(double value, const std::string& from, const std::string& to) {
    if (value < 0.0) return std::nullopt; // Mass cannot be negative
    std::string fromLower = toLower(from);
    std::string toLowerStr = toLower(to);

    auto itFrom = kMassToGrams.find(fromLower);
    auto itTo = kMassToGrams.find(toLowerStr);
    if (itFrom == kMassToGrams.end() || itTo == kMassToGrams.end()) {
        return std::nullopt;
    }
    double inGrams = value * itFrom->second;
    return inGrams / itTo->second;
}

// Temperature: base is Celsius
std::optional<double> convertTemperature(double value, const std::string& from, const std::string& to) {
    std::string fromLower = toLower(from);
    std::string toLowerStr = toLower(to);

    // Convert to Celsius first
    double celsius = 0.0;
    if (fromLower == "c" || fromLower == "celsius") {
        celsius = value;
    } else if (fromLower == "f" || fromLower == "fahrenheit") {
        celsius = (value - 32.0) * 5.0 / 9.0;
    } else if (fromLower == "k" || fromLower == "kelvin") {
        if (value < 0.0) return std::nullopt; // Kelvin cannot be negative
        celsius = value - 273.15;
    } else {
        return std::nullopt;
    }

    // Absolute zero check: -273.15 C
    if (celsius < -273.15) {
        return std::nullopt;
    }

    // Convert from Celsius to target
    if (toLowerStr == "c" || toLowerStr == "celsius") {
        return celsius;
    } else if (toLowerStr == "f" || toLowerStr == "fahrenheit") {
        return (celsius * 9.0 / 5.0) + 32.0;
    } else if (toLowerStr == "k" || toLowerStr == "kelvin") {
        return celsius + 273.15;
    }

    return std::nullopt;
}

// Speed: base is meters per second (mps)
static const std::unordered_map<std::string, double> kSpeedToMps = {
    {"mps", 1.0},
    {"m/s", 1.0},
    {"kmh", 1.0 / 3.6},
    {"km/h", 1.0 / 3.6},
    {"mph", 0.44704},
    {"knot", 0.514444},
    {"knots", 0.514444}
};

std::optional<double> convertSpeed(double value, const std::string& from, const std::string& to) {
    if (value < 0.0) return std::nullopt;
    std::string fromLower = toLower(from);
    std::string toLowerStr = toLower(to);

    auto itFrom = kSpeedToMps.find(fromLower);
    auto itTo = kSpeedToMps.find(toLowerStr);
    if (itFrom == kSpeedToMps.end() || itTo == kSpeedToMps.end()) {
        return std::nullopt;
    }
    double inMps = value * itFrom->second;
    return inMps / itTo->second;
}

std::optional<ConversionResult> convert(Category category, double value, const std::string& from, const std::string& to) {
    std::optional<double> res;
    switch (category) {
        case Category::Length: res = convertLength(value, from, to); break;
        case Category::Mass: res = convertMass(value, from, to); break;
        case Category::Temperature: res = convertTemperature(value, from, to); break;
        case Category::Speed: res = convertSpeed(value, from, to); break;
    }
    if (!res) return std::nullopt;

    ConversionResult cr;
    cr.originalValue = value;
    cr.fromUnit = from;
    cr.convertedValue = *res;
    cr.toUnit = to;
    cr.category = category;
    return cr;
}

} // namespace unit_converter
