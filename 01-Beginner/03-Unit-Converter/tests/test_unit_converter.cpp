#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest/doctest.h"
#include "UnitConverter.h"

using namespace unit_converter;

TEST_CASE("Length conversions") {
    auto kmToM = convertLength(1.0, "km", "m");
    REQUIRE(kmToM.has_value());
    CHECK(*kmToM == doctest::Approx(1000.0));

    auto mToCm = convertLength(1.5, "m", "cm");
    REQUIRE(mToCm.has_value());
    CHECK(*mToCm == doctest::Approx(150.0));

    auto miToKm = convertLength(1.0, "mi", "km");
    REQUIRE(miToKm.has_value());
    CHECK(*miToKm == doctest::Approx(1.609344));

    auto inToCm = convertLength(1.0, "in", "cm");
    REQUIRE(inToCm.has_value());
    CHECK(*inToCm == doctest::Approx(2.54));

    // Negative length check
    CHECK(convertLength(-5.0, "m", "km").has_value() == false);
}

TEST_CASE("Mass conversions") {
    auto kgToG = convertMass(2.5, "kg", "g");
    REQUIRE(kgToG.has_value());
    CHECK(*kgToG == doctest::Approx(2500.0));

    auto lbToKg = convertMass(1.0, "lb", "kg");
    REQUIRE(lbToKg.has_value());
    CHECK(*lbToKg == doctest::Approx(0.453592).epsilon(0.001));

    // Negative mass check
    CHECK(convertMass(-1.0, "kg", "lb").has_value() == false);
}

TEST_CASE("Temperature conversions and absolute zero boundary") {
    // Water freezing point
    auto c0ToF = convertTemperature(0.0, "c", "f");
    REQUIRE(c0ToF.has_value());
    CHECK(*c0ToF == doctest::Approx(32.0));

    // Water boiling point
    auto c100ToF = convertTemperature(100.0, "c", "f");
    REQUIRE(c100ToF.has_value());
    CHECK(*c100ToF == doctest::Approx(212.0));

    // Absolute zero in Kelvin
    auto k0ToC = convertTemperature(0.0, "k", "c");
    REQUIRE(k0ToC.has_value());
    CHECK(*k0ToC == doctest::Approx(-273.15));

    // Below absolute zero is rejected
    CHECK(convertTemperature(-1.0, "k", "c").has_value() == false);
    CHECK(convertTemperature(-300.0, "c", "f").has_value() == false);
}

TEST_CASE("Speed conversions") {
    auto mpsToKmh = convertSpeed(10.0, "mps", "kmh");
    REQUIRE(mpsToKmh.has_value());
    CHECK(*mpsToKmh == doctest::Approx(36.0));

    auto mphToKmh = convertSpeed(60.0, "mph", "kmh");
    REQUIRE(mphToKmh.has_value());
    CHECK(*mphToKmh == doctest::Approx(96.5606).epsilon(0.01));

    CHECK(convertSpeed(-10.0, "mps", "kmh").has_value() == false);
}

TEST_CASE("Category parsing and invalid units") {
    CHECK(parseCategory("length") == Category::Length);
    CHECK(parseCategory("mass") == Category::Mass);
    CHECK(parseCategory("temperature") == Category::Temperature);
    CHECK(parseCategory("speed") == Category::Speed);
    CHECK(parseCategory("unknown").has_value() == false);

    // Invalid unit within category
    CHECK(convert(Category::Length, 10.0, "m", "celsius").has_value() == false);
}
