cmake_minimum_required(VERSION 3.20)
set(CMAKE_POLICY_VERSION_MINIMUM 3.5 CACHE STRING "" FORCE)

project(PDFGeneratorApp VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(MSVC)
    add_compile_options(/W4 /permissive-)
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

add_library(pdf_core STATIC
    src/PdfDocument.cpp
)
target_include_directories(pdf_core PUBLIC
    include
)

add_executable(PDFGeneratorApp
    src/main.cpp
)
target_link_libraries(PDFGeneratorApp PRIVATE pdf_core)

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

add_executable(PDFGeneratorApp_unit_tests
    tests/test_pdf_generator.cpp
)
target_link_libraries(PDFGeneratorApp_unit_tests PRIVATE pdf_core doctest::doctest)

add_test(NAME PDFGeneratorUnitTests COMMAND PDFGeneratorApp_unit_tests)
