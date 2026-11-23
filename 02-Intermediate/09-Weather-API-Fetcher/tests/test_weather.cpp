#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>
#include "WeatherService.hpp"

TEST_CASE("WeatherService - JSON Parsing & Deserialization") {
    std::string validJson = R"({
        "coord": {"lon": 139.6917, "lat": 35.6895},
        "weather": [{"id": 800, "main": "Clear", "description": "clear sky", "icon": "01d"}],
        "main": {"temp": 18.5, "feels_like": 17.9, "humidity": 55},
        "wind": {"speed": 3.6},
        "sys": {"country": "JP"},
        "name": "Tokyo",
        "cod": 200
    })";

    auto report = weather::WeatherService::parseOpenWeatherJson(validJson);
    REQUIRE(report.has_value());
    CHECK(report->city == "Tokyo");
    CHECK(report->country == "JP");
    CHECK(report->coords.latitude == doctest::Approx(35.6895));
    CHECK(report->coords.longitude == doctest::Approx(139.6917));
    CHECK(report->temperature == doctest::Approx(18.5));
    CHECK(report->feelsLike == doctest::Approx(17.9));
    CHECK(report->humidity == 55);
    CHECK(report->windSpeed == doctest::Approx(3.6));
    CHECK(report->conditionSummary == "clear sky");
    CHECK(report->isMock == false);
}

TEST_CASE("WeatherService - Malformed & Error JSON Payloads") {
    std::string notFoundJson = R"({
        "cod": 404,
        "message": "city not found"
    })";

    auto errReport = weather::WeatherService::parseOpenWeatherJson(notFoundJson);
    CHECK(errReport.has_value() == false);

    auto corruptedReport = weather::WeatherService::parseOpenWeatherJson("{invalid json");
    CHECK(corruptedReport.has_value() == false);
}

TEST_CASE("WeatherService - Mock Feed Generation") {
    auto mock = weather::WeatherService::generateMockReport("Paris", "metric");
    CHECK(mock.city == "Paris");
    CHECK(mock.isMock == true);
    CHECK(mock.temperature == doctest::Approx(20.0));
    CHECK(mock.humidity == 65);

    auto mockImperial = weather::WeatherService::generateMockReport("Chicago", "imperial");
    CHECK(mockImperial.city == "Chicago");
    CHECK(mockImperial.temperature == doctest::Approx(68.0));
}
