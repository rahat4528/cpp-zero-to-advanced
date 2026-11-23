cmake_minimum_required(VERSION 3.20)
set(CMAKE_POLICY_VERSION_MINIMUM 3.5 CACHE STRING "" FORCE)

project(QuizTimerApp VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(MSVC)
    add_compile_options(/W4 /permissive-)
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

add_library(quiz_timer_core STATIC
    src/QuizTimerEngine.cpp
)
target_include_directories(quiz_timer_core PUBLIC
    include
)

add_executable(QuizTimerApp
    src/main.cpp
)
target_link_libraries(QuizTimerApp PRIVATE quiz_timer_core)

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

add_executable(QuizTimerApp_unit_tests
    tests/test_quiz_timer.cpp
)
target_link_libraries(QuizTimerApp_unit_tests PRIVATE quiz_timer_core doctest::doctest)

add_test(NAME QuizTimerUnitTests COMMAND QuizTimerApp_unit_tests)
