cmake_minimum_required(VERSION 3.20)
set(CMAKE_POLICY_VERSION_MINIMUM 3.5 CACHE STRING "" FORCE)

project(ImageFilterTool VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(MSVC)
    add_compile_options(/W4 /permissive-)
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

add_library(image_core STATIC
    src/Image.cpp
    src/ImageFilter.cpp
)
target_include_directories(image_core PUBLIC
    include
)

add_executable(ImageFilterTool
    src/main.cpp
)
target_link_libraries(ImageFilterTool PRIVATE image_core)

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

add_executable(ImageFilterTool_unit_tests
    tests/test_image_filter.cpp
)
target_link_libraries(ImageFilterTool_unit_tests PRIVATE image_core doctest::doctest)

add_test(NAME ImageFilterToolUnitTests COMMAND ImageFilterTool_unit_tests)
