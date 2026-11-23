cmake_minimum_required(VERSION 3.20)
set(CMAKE_POLICY_VERSION_MINIMUM 3.5 CACHE STRING "" FORCE)

project(FaceDetectionApp VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(MSVC)
    add_compile_options(/W4 /permissive-)
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

find_package(OpenCV QUIET)

add_library(face_core STATIC
    src/FaceDetector.cpp
)
target_include_directories(face_core PUBLIC
    include
)

if(OpenCV_FOUND)
    target_link_libraries(face_core PUBLIC ${OpenCV_LIBS})
    target_compile_definitions(face_core PUBLIC HAVE_OPENCV)
endif()

add_executable(FaceDetectionApp
    src/main.cpp
)
target_link_libraries(FaceDetectionApp PRIVATE face_core)

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

add_executable(FaceDetectionApp_unit_tests
    tests/test_face_detector.cpp
)
target_link_libraries(FaceDetectionApp_unit_tests PRIVATE face_core doctest::doctest)

add_test(NAME FaceDetectionUnitTests COMMAND FaceDetectionApp_unit_tests)
