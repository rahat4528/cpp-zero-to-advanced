#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest/doctest.h"
#include "TemperatureConverter.h"

using namespace temperature_converter;

TEST_CASE("Water freezing and boiling points") {
    // 0 °C
    auto freeze = TemperatureConverter::convert(0.0, Scale::Celsius);
    REQUIRE(freeze.has_value());
    CHECK(freeze->celsius == doctest::Approx(0.0));
    CHECK(freeze->fahrenheit == doctest::Approx(32.0));
    CHECK(freeze->kelvin == doctest::Approx(273.15));
    CHECK(freeze->rankine == doctest::Approx(491.67));

    // 100 °C
    auto boil = TemperatureConverter::convert(100.0, Scale::Celsius);
    REQUIRE(boil.has_value());
    CHECK(boil->celsius == doctest::Approx(100.0));
    CHECK(boil->fahrenheit == doctest::Approx(212.0));
    CHECK(boil->kelvin == doctest::Approx(373.15));
}

TEST_CASE("Body temperature") {
    // 98.6 °F
    auto body = TemperatureConverter::convert(98.6, Scale::Fahrenheit);
    REQUIRE(body.has_value());
    CHECK(body->celsius == doctest::Approx(37.0).epsilon(0.01));
    CHECK(body->kelvin == doctest::Approx(310.15).epsilon(0.01));
}

TEST_CASE("Absolute zero barrier and rejection") {
    // 0 Kelvin
    auto absZero = TemperatureConverter::convert(0.0, Scale::Kelvin);
    REQUIRE(absZero.has_value());
    CHECK(absZero->celsius == doctest::Approx(-273.15));
    CHECK(absZero->fahrenheit == doctest::Approx(-459.67));
    CHECK(absZero->rankine == doctest::Approx(0.0));

    // Rejection of sub-absolute-zero values
    CHECK(TemperatureConverter::convert(-1.0, Scale::Kelvin).has_value() == false);
    CHECK(TemperatureConverter::convert(-273.16, Scale::Celsius).has_value() == false);
    CHECK(TemperatureConverter::convert(-460.0, Scale::Fahrenheit).has_value() == false);
}

TEST_CASE("Thermometer gauge rendering") {
    std::string coldGauge = TemperatureConverter::renderThermometerGauge(-20.0);
    CHECK(coldGauge.find("[|") != std::string::npos);

    std::string hotGauge = TemperatureConverter::renderThermometerGauge(120.0);
    CHECK(hotGauge.find("====]") != std::string::npos);
}
