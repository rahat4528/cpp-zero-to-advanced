cmake_minimum_required(VERSION 3.20)
set(CMAKE_POLICY_VERSION_MINIMUM 3.5 CACHE STRING "" FORCE)

project(CarRentalSystem VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(MSVC)
    add_compile_options(/W4 /permissive-)
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

find_package(SQLite3 REQUIRED)

add_library(rental_core STATIC
    src/Vehicle.cpp
    src/RentalSystem.cpp
)
target_include_directories(rental_core PUBLIC
    include
)
target_link_libraries(rental_core PUBLIC SQLite3::SQLite3)

add_executable(CarRentalSystem
    src/main.cpp
)
target_link_libraries(CarRentalSystem PRIVATE rental_core)

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

add_executable(CarRentalSystem_tests
    tests/test_rental_system.cpp
)
target_link_libraries(CarRentalSystem_tests PRIVATE rental_core doctest::doctest)

add_test(NAME CarRentalUnitTests COMMAND CarRentalSystem_tests)
