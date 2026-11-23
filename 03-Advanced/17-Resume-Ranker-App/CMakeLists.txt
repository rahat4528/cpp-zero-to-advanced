cmake_minimum_required(VERSION 3.20)
set(CMAKE_POLICY_VERSION_MINIMUM 3.5 CACHE STRING "" FORCE)

project(ResumeRankerApp LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if (MSVC)
    add_compile_options(/W4 /permissive-)
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

include_directories(include)

add_library(ranker_core STATIC
    src/TextTokenizer.cpp
    src/TFIDFModel.cpp
)

add_executable(ResumeRanker src/main.cpp)
target_link_libraries(ResumeRanker PRIVATE ranker_core)

enable_testing()

find_path(DOCTEST_INCLUDE_DIR "doctest/doctest.h"
    PATHS "C:/msys64/mingw64/include"
)

if (DOCTEST_INCLUDE_DIR)
    include_directories(${DOCTEST_INCLUDE_DIR})
    add_executable(test_resume_ranker tests/test_resume_ranker.cpp)
    target_link_libraries(test_resume_ranker PRIVATE ranker_core)
    add_test(NAME TestResumeRanker COMMAND test_resume_ranker)
endif()
