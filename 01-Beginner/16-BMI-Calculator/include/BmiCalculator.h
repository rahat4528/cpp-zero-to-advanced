#pragma once

#include <optional>
#include <string>
#include <utility>

namespace bmi_calculator {

enum class BmiCategory {
    SevereThinness,
    ModerateThinness,
    MildThinness,
    NormalWeight,
    Overweight,
    ObeseClassI,
    ObeseClassII,
    ObeseClassIII
};

struct BmiResult {
    double bmiValue{0.0};
    BmiCategory category{BmiCategory::NormalWeight};
    std::string categoryName;
    std::string healthRisk;
    double idealWeightMinKg{0.0};
    double idealWeightMaxKg{0.0};
    double idealWeightMinLbs{0.0};
    double idealWeightMaxLbs{0.0};
};

class BmiCalculator {
public:
    // Metric: weight in kilograms, height in meters
    static std::optional<BmiResult> calculateMetric(double weightKg, double heightMeters);

    // Imperial: weight in pounds (lbs), height in inches
    static std::optional<BmiResult> calculateImperial(double weightLbs, double heightInches);

    // WHO Category classification
    static BmiCategory classify(double bmi);
    static std::string categoryToString(BmiCategory cat);
    static std::string categoryHealthRisk(BmiCategory cat);

    // Ideal healthy weight bounds for height (BMI 18.5 - 24.9)
    static std::pair<double, double> getIdealWeightRangeKg(double heightMeters);
};

} // namespace bmi_calculator
