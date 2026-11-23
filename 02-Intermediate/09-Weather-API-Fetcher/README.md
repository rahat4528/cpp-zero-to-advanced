# Project 09: Weather-API-Fetcher

## 1. Project Overview
`Weather-API-Fetcher` is a weather telemetry client that queries real-time meteorological conditions via RESTful HTTPS APIs, deserializes JSON data into native C++ structures using `nlohmann/json`, and renders formatted weather dashboards.

## 2. Problem Statement
Networked software applications must securely communicate with external HTTPS endpoints, read configuration and API keys from environment variables rather than hardcoded credentials, parse schema-varying JSON structures, and gracefully fallback to mock or cached data when network connectivity is absent.

## 3. Objectives
- Implement an HTTP/HTTPS client architecture reading `WEATHER_API_KEY` from the system environment with `.env.example` guidance.
- Deserialize JSON responses (temperature, coordinates, humidity, wind velocity, weather condition text) using `nlohmann/json`.
- Provide automated offline mock generators allowing complete test pass rates in offline CI/CTest environments.
- Provide clean cross-platform network socket/HTTP layers (WinINet on Windows and libcurl on Linux).

## 4. Features
- **JSON Deserialization**: Robust parsing of nested JSON fields with type safety and default fallbacks.
- **Credential Safety**: No hardcoded API keys; loads keys from `WEATHER_API_KEY` environment variable.
- **Dual Unit Modes**: Supports Metric (Celsius, m/s) and Imperial (Fahrenheit, mph) queries.
- **Deterministic Offline Testing**: Built-in mock data engine allowing 100% test coverage without live internet access.

## 5. Technology Stack
- **Language**: C++17
- **Compiler**: GCC 11+, Clang 14+, MSVC 2022
- **Build System**: CMake 3.20+
- **JSON Parser**: `nlohmann/json` 3.11+
- **Networking**: WinINet (native Windows HTTPS) / libcurl
- **Testing**: doctest 2.4+ via CTest
- **External Services**: OpenWeatherMap API (optional for live feed; offline mock fallback included)

## 6. Folder Structure
```
09-Weather-API-Fetcher/
├── include/
│   └── WeatherService.hpp
├── src/
│   ├── WeatherService.cpp
│   └── main.cpp
├── tests/
│   └── test_weather.cpp
├── CMakeLists.txt
├── vcpkg.json
├── .env.example
├── README.md
└── .gitignore
```

## 7. Installation / Prerequisites
- CMake 3.20+
- C++17 compliant compiler
- On Windows: WinINet (standard Windows SDK)
- On Linux: `libcurl4-openssl-dev`

## 8. Build and Run Instructions
### Windows & Linux
```bash
cmake -S . -B build -G Ninja
cmake --build build

# Run with simulated mock or environment key:
./build/WeatherAPIFetcher

# Query specific city with units:
./build/WeatherAPIFetcher Tokyo metric

# Run headless automated verification:
./build/WeatherAPIFetcher --test-run
```

## 9. Testing Instructions
```bash
ctest --test-dir build --output-on-failure
```

## 10. Console Output Sample
```
=================================================================
 CURRENT WEATHER: Zurich (Mock Feed)
=================================================================
 Condition    : Scattered Clouds (Simulated Offline Data)
 Temperature  : 20.0 deg C (Feels like: 19.2 deg C)
 Humidity     : 65 %
 Wind Speed   : 4.2 m/s
 Location     : Lat 51.5074, Lon -0.1278
 Mode         : Mock / Offline Mode
=================================================================
```

## 11. Known Limitations
- Live querying requires an active OpenWeatherMap API key exported in `WEATHER_API_KEY`. Without an active key, the engine defaults to the offline mock dataset.

## 12. Testing Status
- **Built**: Yes (Compiled cleanly with GCC 16.1.0 on Windows, 0 warnings).
- **Run**: Verified both live/mock execution and `--test-run` flag.
- **Tested**: 3 test suites covering valid JSON deserialization, malformed payload error handling, and mock generation passing under CTest.
