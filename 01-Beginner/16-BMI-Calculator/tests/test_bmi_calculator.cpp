#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest/doctest.h"
#include "BmiCalculator.h"

using namespace bmi_calculator;

TEST_CASE("Metric BMI calculations") {
    // 70 kg, 1.75 m -> BMI = 70 / (1.75^2) = 22.857
    auto res = BmiCalculator::calculateMetric(70.0, 1.75);
    REQUIRE(res.has_value());
    CHECK(res->bmiValue == doctest::Approx(22.857).epsilon(0.01));
    CHECK(res->category == BmiCategory::NormalWeight);
}

TEST_CASE("Imperial BMI calculations") {
    // 160 lbs, 68 inches -> approx 72.57 kg, 1.727 m -> BMI = 24.32
    auto res = BmiCalculator::calculateImperial(160.0, 68.0);
    REQUIRE(res.has_value());
    CHECK(res->bmiValue == doctest::Approx(24.32).epsilon(0.05));
    CHECK(res->category == BmiCategory::NormalWeight);
}

TEST_CASE("WHO Category classifications") {
    CHECK(BmiCalculator::classify(15.5) == BmiCategory::SevereThinness);
    CHECK(BmiCalculator::classify(16.5) == BmiCategory::ModerateThinness);
    CHECK(BmiCalculator::classify(17.5) == BmiCategory::MildThinness);
    CHECK(BmiCalculator::classify(22.0) == BmiCategory::NormalWeight);
    CHECK(BmiCalculator::classify(27.0) == BmiCategory::Overweight);
    CHECK(BmiCalculator::classify(32.0) == BmiCategory::ObeseClassI);
    CHECK(BmiCalculator::classify(37.0) == BmiCategory::ObeseClassII);
    CHECK(BmiCalculator::classify(45.0) == BmiCategory::ObeseClassIII);
}

TEST_CASE("Ideal weight range calculation") {
    auto [minKg, maxKg] = BmiCalculator::getIdealWeightRangeKg(1.80);
    // 18.5 * 1.8^2 = 59.94 kg
    // 24.9 * 1.8^2 = 80.676 kg
    CHECK(minKg == doctest::Approx(59.94).epsilon(0.01));
    CHECK(maxKg == doctest::Approx(80.676).epsilon(0.01));
}

TEST_CASE("Negative and implausible boundary rejections") {
    CHECK(BmiCalculator::calculateMetric(-70.0, 1.75).has_value() == false);
    CHECK(BmiCalculator::calculateMetric(70.0, 0.0).has_value() == false);
    CHECK(BmiCalculator::calculateMetric(70.0, -1.5).has_value() == false);
    CHECK(BmiCalculator::calculateMetric(1000.0, 1.75).has_value() == false); // Beyond max human weight
}
