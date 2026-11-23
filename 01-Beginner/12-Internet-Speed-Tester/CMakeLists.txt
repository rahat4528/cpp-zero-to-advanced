cmake_minimum_required(VERSION 3.20)
project(InternetSpeedTester CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(MSVC)
    add_compile_options(/W4 /permissive-)
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

add_library(speed_tester_core
    src/SpeedTester.cpp
)
target_include_directories(speed_tester_core PUBLIC include)

if(WIN32)
    target_link_libraries(speed_tester_core PUBLIC wininet)
endif()

add_executable(SpeedTester
    src/main.cpp
)
target_link_libraries(SpeedTester PRIVATE speed_tester_core)

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

add_executable(SpeedTester_tests
    tests/test_speed_tester.cpp
)
target_link_libraries(SpeedTester_tests PRIVATE speed_tester_core doctest::doctest)

include(CTest)
add_test(NAME SpeedTesterUnitTests COMMAND SpeedTester_tests)
