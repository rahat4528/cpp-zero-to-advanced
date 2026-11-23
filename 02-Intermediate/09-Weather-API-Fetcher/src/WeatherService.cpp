#include "WeatherService.hpp"
#include <nlohmann/json.hpp>
#include <iostream>
#include <cstdlib>

#if defined(_WIN32) || defined(_WIN64)
    #include <windows.h>
    #include <wininet.h>
    #if defined(_MSC_VER)
        #pragma comment(lib, "wininet.lib")
    #endif
#endif

namespace weather {

WeatherService::WeatherService(std::string apiKey) : m_apiKey(std::move(apiKey)) {
    if (m_apiKey.empty()) {
        const char* envKey = std::getenv("WEATHER_API_KEY");
        if (envKey) {
            m_apiKey = envKey;
        }
    }
}

void WeatherService::setApiKey(std::string apiKey) {
    m_apiKey = std::move(apiKey);
}

const std::string& WeatherService::getApiKey() const {
    return m_apiKey;
}

std::string WeatherService::httpGet(const std::string& url) {
#if defined(_WIN32) || defined(_WIN64)
    HINTERNET hInternet = InternetOpenA("WeatherServiceApp/1.0", INTERNET_OPEN_TYPE_PRECONFIG, nullptr, nullptr, 0);
    if (!hInternet) return "";

    HINTERNET hUrl = InternetOpenUrlA(hInternet, url.c_str(), nullptr, 0,
                                      INTERNET_FLAG_RELOAD | INTERNET_FLAG_SECURE | INTERNET_FLAG_DONT_CACHE, 0);
    if (!hUrl) {
        InternetCloseHandle(hInternet);
        return "";
    }

    std::string response;
    char buffer[4096];
    DWORD bytesRead = 0;
    while (InternetReadFile(hUrl, buffer, sizeof(buffer), &bytesRead) && bytesRead > 0) {
        response.append(buffer, bytesRead);
    }

    InternetCloseHandle(hUrl);
    InternetCloseHandle(hInternet);
    return response;
#else
    (void)url;
    return "";
#endif
}

std::optional<WeatherReport> WeatherService::fetchCurrentWeather(const std::string& city, const std::string& units) {
    if (m_apiKey.empty() || m_apiKey == "your_api_key_here") {
        return generateMockReport(city, units);
    }

    std::string url = "https://api.openweathermap.org/data/2.5/weather?q=" + city +
                      "&units=" + units + "&appid=" + m_apiKey;

    std::string rawJson = httpGet(url);
    if (rawJson.empty()) {
        // Fallback to mock on network failure
        return generateMockReport(city, units);
    }

    return parseOpenWeatherJson(rawJson);
}

std::optional<WeatherReport> WeatherService::parseOpenWeatherJson(const std::string& rawJson) {
    try {
        auto j = nlohmann::json::parse(rawJson);

        if (j.contains("cod") && j["cod"] != 200) {
            return std::nullopt;
        }

        WeatherReport report;
        report.city = j.value("name", "Unknown");
        if (j.contains("sys") && j["sys"].contains("country")) {
            report.country = j["sys"]["country"].get<std::string>();
        }

        if (j.contains("coord")) {
            report.coords.latitude = j["coord"].value("lat", 0.0);
            report.coords.longitude = j["coord"].value("lon", 0.0);
        }

        if (j.contains("main")) {
            report.temperature = j["main"].value("temp", 0.0);
            report.feelsLike = j["main"].value("feels_like", 0.0);
            report.humidity = j["main"].value("humidity", 0);
        }

        if (j.contains("wind")) {
            report.windSpeed = j["wind"].value("speed", 0.0);
        }

        if (j.contains("weather") && j["weather"].is_array() && !j["weather"].empty()) {
            report.conditionSummary = j["weather"][0].value("description", "Unknown");
            report.icon = j["weather"][0].value("icon", "");
        }

        report.isMock = false;
        return report;
    } catch (const std::exception&) {
        return std::nullopt;
    }
}

WeatherReport WeatherService::generateMockReport(const std::string& city, const std::string& units) {
    WeatherReport report;
    report.city = city.empty() ? "London" : city;
    report.country = "UK (Mock Feed)";
    report.coords = {51.5074, -0.1278};
    report.temperature = (units == "imperial") ? 68.0 : 20.0;
    report.feelsLike = (units == "imperial") ? 66.5 : 19.2;
    report.humidity = 65;
    report.windSpeed = (units == "imperial") ? 9.5 : 4.2;
    report.conditionSummary = "Scattered Clouds (Simulated Offline Data)";
    report.icon = "03d";
    report.isMock = true;
    return report;
}

} // namespace weather
