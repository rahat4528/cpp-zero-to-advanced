#include "WeatherService.hpp"
#include <iostream>
#include <iomanip>
#include <string>

int main(int argc, char* argv[]) {
    std::string city = "London";
    std::string units = "metric";

    if (argc >= 2 && std::string(argv[1]) == "--test-run") {
        std::cout << "=================================================================\n";
        std::cout << "                WEATHER API FETCHER (HTTP & JSON)                \n";
        std::cout << "=================================================================\n";
        std::cout << "[Test-Run Mode] Executing weather query with mock fallback...\n";

        weather::WeatherService svc;
        auto report = svc.fetchCurrentWeather("Zurich", "metric");
        if (report) {
            std::cout << "Weather Report for " << report->city << ", " << report->country << ":\n";
            std::cout << "  - Coordinates : (" << report->coords.latitude << ", " << report->coords.longitude << ")\n";
            std::cout << "  - Temperature : " << report->temperature << " C (Feels like: " << report->feelsLike << " C)\n";
            std::cout << "  - Humidity    : " << report->humidity << " %\n";
            std::cout << "  - Wind Speed  : " << report->windSpeed << " m/s\n";
            std::cout << "  - Condition   : " << report->conditionSummary << "\n";
            std::cout << "  - Data Source : " << (report->isMock ? "MOCK OFFLINE" : "LIVE HTTP API") << "\n";
        }
        return 0;
    }

    if (argc >= 2) city = argv[1];
    if (argc >= 3) units = argv[2];

    weather::WeatherService svc;
    auto report = svc.fetchCurrentWeather(city, units);

    if (!report) {
        std::cerr << "Failed to fetch weather data for '" << city << "'. Check API key and internet connection.\n";
        return 1;
    }

    std::string tempUnit = (units == "imperial") ? "F" : "C";
    std::string speedUnit = (units == "imperial") ? "mph" : "m/s";

    std::cout << "\n=================================================================\n";
    std::cout << " CURRENT WEATHER: " << report->city << " (" << report->country << ")\n";
    std::cout << "=================================================================\n";
    std::cout << " Condition    : " << report->conditionSummary << "\n";
    std::cout << " Temperature  : " << std::fixed << std::setprecision(1) << report->temperature << " deg " << tempUnit
              << " (Feels like: " << report->feelsLike << " deg " << tempUnit << ")\n";
    std::cout << " Humidity     : " << report->humidity << " %\n";
    std::cout << " Wind Speed   : " << report->windSpeed << " " << speedUnit << "\n";
    std::cout << " Location     : Lat " << report->coords.latitude << ", Lon " << report->coords.longitude << "\n";
    std::cout << " Mode         : " << (report->isMock ? "Mock / Offline Mode" : "Live API") << "\n";
    std::cout << "=================================================================\n";

    return 0;
}
