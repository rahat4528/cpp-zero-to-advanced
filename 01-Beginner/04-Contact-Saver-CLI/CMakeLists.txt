cmake_minimum_required(VERSION 3.20)
project(ContactSaver CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(MSVC)
    add_compile_options(/W4 /permissive-)
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

add_library(contact_saver_core
    src/ContactSaver.cpp
)
target_include_directories(contact_saver_core PUBLIC include)

add_executable(ContactSaver
    src/main.cpp
)
target_link_libraries(ContactSaver PRIVATE contact_saver_core)

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

add_executable(ContactSaver_tests
    tests/test_contact_saver.cpp
)
target_link_libraries(ContactSaver_tests PRIVATE contact_saver_core doctest::doctest)

include(CTest)
add_test(NAME ContactSaverUnitTests COMMAND ContactSaver_tests)
