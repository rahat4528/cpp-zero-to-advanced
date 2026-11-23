cmake_minimum_required(VERSION 3.20)
project(EmailValidator CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(MSVC)
    add_compile_options(/W4 /permissive-)
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

add_library(email_validator_core
    src/EmailValidator.cpp
)
target_include_directories(email_validator_core PUBLIC include)

add_executable(EmailValidator
    src/main.cpp
)
target_link_libraries(EmailValidator PRIVATE email_validator_core)

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

add_executable(EmailValidator_tests
    tests/test_email_validator.cpp
)
target_link_libraries(EmailValidator_tests PRIVATE email_validator_core doctest::doctest)

include(CTest)
add_test(NAME EmailValidatorUnitTests COMMAND EmailValidator_tests)
