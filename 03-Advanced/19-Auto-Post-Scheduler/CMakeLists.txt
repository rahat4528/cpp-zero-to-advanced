cmake_minimum_required(VERSION 3.20)
set(CMAKE_POLICY_VERSION_MINIMUM 3.5 CACHE STRING "" FORCE)

project(AutoPostScheduler LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if (MSVC)
    add_compile_options(/W4 /permissive-)
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

find_package(SQLite3 REQUIRED)

include_directories(include)

add_library(scheduler_core STATIC
    src/PostDatabase.cpp
    src/PostScheduler.cpp
)
target_link_libraries(scheduler_core PUBLIC SQLite3::SQLite3)

add_executable(PostSchedulerApp src/main.cpp)
target_link_libraries(PostSchedulerApp PRIVATE scheduler_core)

enable_testing()

find_path(DOCTEST_INCLUDE_DIR "doctest/doctest.h"
    PATHS "C:/msys64/mingw64/include"
)

if (DOCTEST_INCLUDE_DIR)
    include_directories(${DOCTEST_INCLUDE_DIR})
    add_executable(test_post_scheduler tests/test_post_scheduler.cpp)
    target_link_libraries(test_post_scheduler PRIVATE scheduler_core)
    add_test(NAME TestPostScheduler COMMAND test_post_scheduler)
endif()
