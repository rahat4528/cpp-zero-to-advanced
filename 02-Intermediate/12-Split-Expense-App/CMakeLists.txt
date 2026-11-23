cmake_minimum_required(VERSION 3.20)
set(CMAKE_POLICY_VERSION_MINIMUM 3.5 CACHE STRING "" FORCE)

project(SplitExpenseApp VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(MSVC)
    add_compile_options(/W4 /permissive-)
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

add_library(expense_core STATIC
    src/ExpenseManager.cpp
)
target_include_directories(expense_core PUBLIC
    include
)

add_executable(SplitExpenseApp
    src/main.cpp
)
target_link_libraries(SplitExpenseApp PRIVATE expense_core)

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

add_executable(SplitExpenseApp_unit_tests
    tests/test_split_expense.cpp
)
target_link_libraries(SplitExpenseApp_unit_tests PRIVATE expense_core doctest::doctest)

add_test(NAME SplitExpenseUnitTests COMMAND SplitExpenseApp_unit_tests)
