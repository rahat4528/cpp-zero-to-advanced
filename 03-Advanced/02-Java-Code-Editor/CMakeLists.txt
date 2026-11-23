cmake_minimum_required(VERSION 3.20)
set(CMAKE_POLICY_VERSION_MINIMUM 3.5 CACHE STRING "" FORCE)

project(CppCodeEditor VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(MSVC)
    add_compile_options(/W4 /permissive-)
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

add_library(editor_core STATIC
    src/TextBuffer.cpp
    src/CppSyntaxHighlighter.cpp
)
target_include_directories(editor_core PUBLIC
    include
)

add_executable(CppCodeEditor
    src/main.cpp
)
target_link_libraries(CppCodeEditor PRIVATE editor_core)

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

add_executable(CppCodeEditor_unit_tests
    tests/test_code_editor.cpp
)
target_link_libraries(CppCodeEditor_unit_tests PRIVATE editor_core doctest::doctest)

add_test(NAME CppCodeEditorUnitTests COMMAND CppCodeEditor_unit_tests)
