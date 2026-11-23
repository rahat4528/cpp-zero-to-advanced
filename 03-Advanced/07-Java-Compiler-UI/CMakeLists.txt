cmake_minimum_required(VERSION 3.20)
set(CMAKE_POLICY_VERSION_MINIMUM 3.5 CACHE STRING "" FORCE)

project(CppCompilerUI VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(MSVC)
    add_compile_options(/W4 /permissive-)
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

add_library(compiler_core STATIC
    src/CompilerToolchain.cpp
    src/DiagnosticParser.cpp
)
target_include_directories(compiler_core PUBLIC
    include
)

add_executable(CppCompilerUI
    src/main.cpp
)
target_link_libraries(CppCompilerUI PRIVATE compiler_core)

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

add_executable(test_compiler_ui
    tests/test_compiler_ui.cpp
)
target_link_libraries(test_compiler_ui PRIVATE compiler_core doctest::doctest)

add_test(NAME CompilerUITests COMMAND test_compiler_ui)
