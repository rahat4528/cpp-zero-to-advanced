cmake_minimum_required(VERSION 3.20)
project(BillSplitter CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(MSVC)
    add_compile_options(/W4 /permissive-)
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

add_library(bill_splitter_core
    src/BillSplitter.cpp
)
target_include_directories(bill_splitter_core PUBLIC include)

add_executable(BillSplitter
    src/main.cpp
)
target_link_libraries(BillSplitter PRIVATE bill_splitter_core)

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

add_executable(BillSplitter_tests
    tests/test_bill_splitter.cpp
)
target_link_libraries(BillSplitter_tests PRIVATE bill_splitter_core doctest::doctest)

include(CTest)
add_test(NAME BillSplitterUnitTests COMMAND BillSplitter_tests)
