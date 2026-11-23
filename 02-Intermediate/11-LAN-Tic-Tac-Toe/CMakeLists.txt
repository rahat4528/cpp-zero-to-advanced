cmake_minimum_required(VERSION 3.20)
set(CMAKE_POLICY_VERSION_MINIMUM 3.5 CACHE STRING "" FORCE)

project(LANTicTacToe VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(MSVC)
    add_compile_options(/W4 /permissive-)
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

add_library(tictactoe_core STATIC
    src/TicTacToeBoard.cpp
)
target_include_directories(tictactoe_core PUBLIC
    include
)
if(WIN32)
    target_link_libraries(tictactoe_core PUBLIC ws2_32)
endif()

add_executable(TicTacToe
    src/main.cpp
)
target_link_libraries(TicTacToe PRIVATE tictactoe_core)

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

add_executable(TicTacToe_unit_tests
    tests/test_tictactoe.cpp
)
target_link_libraries(TicTacToe_unit_tests PRIVATE tictactoe_core doctest::doctest)

add_test(NAME TicTacToeUnitTests COMMAND TicTacToe_unit_tests)
