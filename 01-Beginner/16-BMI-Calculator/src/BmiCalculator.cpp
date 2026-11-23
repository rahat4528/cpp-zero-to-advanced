#include "BmiCalculator.h"

#include <cmath>

namespace bmi_calculator {

BmiCategory BmiCalculator::classify(double bmi) {
    if (bmi < 16.0) return BmiCategory::SevereThinness;
    if (bmi < 17.0) return BmiCategory::ModerateThinness;
    if (bmi < 18.5) return BmiCategory::MildThinness;
    if (bmi < 25.0) return BmiCategory::NormalWeight;
    if (bmi < 30.0) return BmiCategory::Overweight;
    if (bmi < 35.0) return BmiCategory::ObeseClassI;
    if (bmi < 40.0) return BmiCategory::ObeseClassII;
    return BmiCategory::ObeseClassIII;
}

std::string BmiCalculator::categoryToString(BmiCategory cat) {
    switch (cat) {
        case BmiCategory::SevereThinness: return "Underweight (Severe thinness)";
        case BmiCategory::ModerateThinness: return "Underweight (Moderate thinness)";
        case BmiCategory::MildThinness: return "Underweight (Mild thinness)";
        case BmiCategory::NormalWeight: return "Normal weight (Healthy)";
        case BmiCategory::Overweight: return "Overweight (Pre-obese)";
        case BmiCategory::ObeseClassI: return "Obese (Class I)";
        case BmiCategory::ObeseClassII: return "Obese (Class II)";
        case BmiCategory::ObeseClassIII: return "Obese (Class III - Extreme)";
    }
    return "Unknown";
}

std::string BmiCalculator::categoryHealthRisk(BmiCategory cat) {
    switch (cat) {
        case BmiCategory::SevereThinness: return "Elevated risk of nutritional deficiency";
        case BmiCategory::ModerateThinness: return "Moderate risk of nutritional deficiency";
        case BmiCategory::MildThinness: return "Low risk";
        case BmiCategory::NormalWeight: return "Minimal / Healthy baseline risk";
        case BmiCategory::Overweight: return "Increased risk of cardiovascular issues";
        case BmiCategory::ObeseClassI: return "High risk of metabolic disease";
        case BmiCategory::ObeseClassII: return "Very high risk of cardiovascular disease";
        case BmiCategory::ObeseClassIII: return "Extremely high risk / Clinical priority";
    }
    return "Unknown";
}

std::pair<double, double> BmiCalculator::getIdealWeightRangeKg(double heightMeters) {
    double minKg = 18.5 * (heightMeters * heightMeters);
    double maxKg = 24.9 * (heightMeters * heightMeters);
    return {minKg, maxKg};
}

std::optional<BmiResult> BmiCalculator::calculateMetric(double weightKg, double heightMeters) {
    // Sanity boundary checks
    if (weightKg <= 2.0 || weightKg > 500.0) return std::nullopt;
    if (heightMeters <= 0.4 || heightMeters > 2.8) return std::nullopt;

    double bmi = weightKg / (heightMeters * heightMeters);
    if (std::isnan(bmi) || std::isinf(bmi)) return std::nullopt;

    BmiResult res;
    res.bmiValue = bmi;
    res.category = classify(bmi);
    res.categoryName = categoryToString(res.category);
    res.healthRisk = categoryHealthRisk(res.category);

    auto [minKg, maxKg] = getIdealWeightRangeKg(heightMeters);
    res.idealWeightMinKg = minKg;
    res.idealWeightMaxKg = maxKg;
    res.idealWeightMinLbs = minKg * 2.20462;
    res.idealWeightMaxLbs = maxKg * 2.20462;

    return res;
}

std::optional<BmiResult> BmiCalculator::calculateImperial(double weightLbs, double heightInches) {
    if (weightLbs <= 4.0 || weightLbs > 1100.0) return std::nullopt;
    if (heightInches <= 15.0 || heightInches > 110.0) return std::nullopt;

    double weightKg = weightLbs * 0.45359237;
    double heightM = heightInches * 0.0254;

    return calculateMetric(weightKg, heightM);
}

} // namespace bmi_calculator
