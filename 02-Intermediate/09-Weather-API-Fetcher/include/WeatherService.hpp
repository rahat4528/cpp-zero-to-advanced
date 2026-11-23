#pragma once

#include <string>
#include <optional>
#include <vector>

namespace weather {

struct Coordinates {
    double latitude{0.0};
    double longitude{0.0};
};

struct WeatherReport {
    std::string city;
    std::string country;
    Coordinates coords;
    double temperature{0.0};      // Celsius or Fahrenheit
    double feelsLike{0.0};
    int humidity{0};              // Percentage [0 - 100]
    double windSpeed{0.0};        // m/s or mph
    std::string conditionSummary; // e.g. "Clear sky", "Light rain"
    std::string icon;
    bool isMock{false};
};

class WeatherService {
public:
    explicit WeatherService(std::string apiKey = "");

    void setApiKey(std::string apiKey);
    [[nodiscard]] const std::string& getApiKey() const;

    // Fetch report by city name (e.g. "Tokyo", "New York")
    std::optional<WeatherReport> fetchCurrentWeather(const std::string& city, const std::string& units = "metric");

    // Parse raw OpenWeather JSON payload
    static std::optional<WeatherReport> parseOpenWeatherJson(const std::string& rawJson);

    // Mock generator for offline/test environments
    static WeatherReport generateMockReport(const std::string& city, const std::string& units = "metric");

private:
    std::string httpGet(const std::string& url);

    std::string m_apiKey;
};

} // namespace weather
