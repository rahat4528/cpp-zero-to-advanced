cmake_minimum_required(VERSION 3.20)
set(CMAKE_POLICY_VERSION_MINIMUM 3.5 CACHE STRING "" FORCE)

project(WeatherAPIFetcher VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(MSVC)
    add_compile_options(/W4 /permissive-)
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

find_package(CURL QUIET)

add_library(weather_core STATIC
    src/WeatherService.cpp
)
target_include_directories(weather_core PUBLIC
    include
)

if(CURL_FOUND)
    target_link_libraries(weather_core PUBLIC CURL::libcurl)
    target_compile_definitions(weather_core PUBLIC USE_LIBCURL)
elseif(WIN32)
    target_link_libraries(weather_core PUBLIC wininet)
endif()

add_executable(WeatherAPIFetcher
    src/main.cpp
)
target_link_libraries(WeatherAPIFetcher PRIVATE weather_core)

enable_testing()

find_package(doctest CONFIG QUIET)
if(NOT doctest_FOUND)
    include(FetchContent)
    FetchContent_Declare(
        doctest
        GIT_REPOSITORY https://github.com/doctest/doctest.git
        GIT_TAG v2.4.11
    )
    FetchContent_MakeAvailable(doctest)
endif()

add_executable(WeatherAPIFetcher_unit_tests
    tests/test_weather.cpp
)
target_link_libraries(WeatherAPIFetcher_unit_tests PRIVATE weather_core doctest::doctest)

add_test(NAME WeatherAPIFetcherUnitTests COMMAND WeatherAPIFetcher_unit_tests)
