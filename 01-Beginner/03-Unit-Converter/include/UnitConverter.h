#pragma once

#include <string>
#include <vector>
#include <optional>

namespace unit_converter {

enum class Category {
    Length,
    Mass,
    Temperature,
    Speed
};

struct ConversionResult {
    double originalValue{0.0};
    std::string fromUnit;
    double convertedValue{0.0};
    std::string toUnit;
    Category category;
};

// Returns all supported unit names for a given category
std::vector<std::string> getSupportedUnits(Category category);

// Parses category string (case-insensitive)
std::optional<Category> parseCategory(const std::string& catStr);

// Core conversion functions
std::optional<double> convertLength(double value, const std::string& from, const std::string& to);
std::optional<double> convertMass(double value, const std::string& from, const std::string& to);
std::optional<double> convertTemperature(double value, const std::string& from, const std::string& to);
std::optional<double> convertSpeed(double value, const std::string& from, const std::string& to);

// Unified conversion dispatcher
std::optional<ConversionResult> convert(Category category, double value, const std::string& from, const std::string& to);

} // namespace unit_converter
