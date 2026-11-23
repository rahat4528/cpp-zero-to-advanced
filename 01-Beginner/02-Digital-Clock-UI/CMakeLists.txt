cmake_minimum_required(VERSION 3.20)
project(DigitalClock CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(MSVC)
    add_compile_options(/W4 /permissive-)
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# Core library
add_library(digital_clock_core
    src/DigitalClock.cpp
)
target_include_directories(digital_clock_core PUBLIC include)

# Main executable
add_executable(DigitalClock
    src/main.cpp
)
target_link_libraries(DigitalClock PRIVATE digital_clock_core)

# Tests
enable_testing()

find_package(doctest CONFIG QUIET)
if(NOT doctest_FOUND)
    include(FetchContent)
    set(CMAKE_POLICY_VERSION_MINIMUM 3.5 CACHE STRING "" FORCE)
    FetchContent_Declare(
        doctest
        GIT_REPOSITORY https://github.com/doctest/doctest.git
        GIT_TAG v2.4.11
        GIT_SHALLOW TRUE
    )
    FetchContent_MakeAvailable(doctest)
endif()

add_executable(DigitalClock_tests
    tests/test_digital_clock.cpp
)
target_link_libraries(DigitalClock_tests PRIVATE digital_clock_core doctest::doctest)

include(CTest)
add_test(NAME DigitalClockUnitTests COMMAND DigitalClock_tests)
