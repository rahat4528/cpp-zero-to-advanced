cmake_minimum_required(VERSION 3.20)
set(CMAKE_POLICY_VERSION_MINIMUM 3.5 CACHE STRING "" FORCE)

project(CodingTestPlatform LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if (MSVC)
    add_compile_options(/W4 /permissive-)
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

include_directories(include)

add_library(judge_core STATIC
    src/ExecutionSandbox.cpp
    src/JudgeEngine.cpp
)

if (WIN32)
    target_link_libraries(judge_core PUBLIC psapi)
endif()

add_executable(CodingTestPlatform src/main.cpp)
target_link_libraries(CodingTestPlatform PRIVATE judge_core)

enable_testing()

find_path(DOCTEST_INCLUDE_DIR "doctest/doctest.h"
    PATHS "C:/msys64/mingw64/include"
)

if (DOCTEST_INCLUDE_DIR)
    include_directories(${DOCTEST_INCLUDE_DIR})
    add_executable(test_judge_platform tests/test_judge_platform.cpp)
    target_link_libraries(test_judge_platform PRIVATE judge_core)
    add_test(NAME TestCodingJudgePlatform COMMAND test_judge_platform)
endif()
