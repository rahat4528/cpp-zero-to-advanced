#pragma once

#include <optional>
#include <string>

namespace temperature_converter {

enum class Scale {
    Celsius,
    Fahrenheit,
    Kelvin,
    Rankine
};

struct TemperatureReading {
    double celsius{0.0};
    double fahrenheit{0.0};
    double kelvin{0.0};
    double rankine{0.0};
    std::string stateDescription; // "Below Absolute Zero", "Freezing", "Cold", "Comfortable", "Hot", "Boiling"
};

class TemperatureConverter {
public:
    // Core conversion from a specified scale
    static std::optional<TemperatureReading> convert(double value, Scale fromScale);

    // Helper scale parsing
    static std::optional<Scale> parseScale(const std::string& scaleStr);
    static std::string scaleToString(Scale scale);

    // Visual ASCII thermometer gauge renderer (e.g. "[======|      ] 22.5 °C")
    static std::string renderThermometerGauge(double celsius);
};

} // namespace temperature_converter
