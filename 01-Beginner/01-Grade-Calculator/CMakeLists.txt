cmake_minimum_required(VERSION 3.20)
project(GradeCalculator CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(MSVC)
    add_compile_options(/W4 /permissive-)
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# Core static library
add_library(grade_calculator_core
    src/GradeCalculator.cpp
)
target_include_directories(grade_calculator_core PUBLIC include)

# Main CLI executable
add_executable(GradeCalculator
    src/main.cpp
)
target_link_libraries(GradeCalculator PRIVATE grade_calculator_core)

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

add_executable(GradeCalculator_tests
    tests/test_grade_calculator.cpp
)
target_link_libraries(GradeCalculator_tests PRIVATE grade_calculator_core doctest::doctest)

include(CTest)
add_test(NAME GradeCalculatorUnitTests COMMAND GradeCalculator_tests)
