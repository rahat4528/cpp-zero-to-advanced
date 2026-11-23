cmake_minimum_required(VERSION 3.20)
set(CMAKE_POLICY_VERSION_MINIMUM 3.5 CACHE STRING "" FORCE)

project(CrimeRecordManager LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if (MSVC)
    add_compile_options(/W4 /permissive-)
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

find_package(SQLite3 REQUIRED)
find_package(OpenSSL REQUIRED)

include_directories(include)

add_library(crime_core STATIC
    src/AuthManager.cpp
    src/CrimeDatabase.cpp
)
target_link_libraries(crime_core PUBLIC SQLite3::SQLite3 OpenSSL::Crypto)

add_executable(CrimeRecordManager src/main.cpp)
target_link_libraries(CrimeRecordManager PRIVATE crime_core)

enable_testing()

find_path(DOCTEST_INCLUDE_DIR "doctest/doctest.h"
    PATHS "C:/msys64/mingw64/include"
)

if (DOCTEST_INCLUDE_DIR)
    include_directories(${DOCTEST_INCLUDE_DIR})
    add_executable(test_crime_manager tests/test_crime_manager.cpp)
    target_link_libraries(test_crime_manager PRIVATE crime_core)
    add_test(NAME TestCrimeManager COMMAND test_crime_manager)
endif()
