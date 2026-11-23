cmake_minimum_required(VERSION 3.20)
set(CMAKE_POLICY_VERSION_MINIMUM 3.5 CACHE STRING "" FORCE)

project(ReminderAlarmApp VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(MSVC)
    add_compile_options(/W4 /permissive-)
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

find_package(Threads REQUIRED)

add_library(alarm_core STATIC
    src/AlarmScheduler.cpp
)
target_include_directories(alarm_core PUBLIC
    include
)
target_link_libraries(alarm_core PUBLIC Threads::Threads)
if(WIN32)
    target_link_libraries(alarm_core PUBLIC user32)
endif()

add_executable(ReminderAlarmApp
    src/main.cpp
)
target_link_libraries(ReminderAlarmApp PRIVATE alarm_core)

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

add_executable(ReminderAlarmApp_unit_tests
    tests/test_alarm_scheduler.cpp
)
target_link_libraries(ReminderAlarmApp_unit_tests PRIVATE alarm_core doctest::doctest)

add_test(NAME ReminderAlarmUnitTests COMMAND ReminderAlarmApp_unit_tests)
