cmake_minimum_required(VERSION 3.20)
set(CMAKE_POLICY_VERSION_MINIMUM 3.5 CACHE STRING "" FORCE)

project(OnlineBankingSystem LANGUAGES CXX)

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

add_library(banking_core STATIC
    src/BankingDatabase.cpp
    src/BankingServer.cpp
    src/BankingClient.cpp
)
target_link_libraries(banking_core PUBLIC SQLite3::SQLite3)

if (WIN32)
    target_link_libraries(banking_core PUBLIC ws2_32)
endif()

add_executable(BankingServer src/server_main.cpp)
target_link_libraries(BankingServer PRIVATE banking_core)

add_executable(BankingClient src/client_main.cpp)
target_link_libraries(BankingClient PRIVATE banking_core)

enable_testing()

find_path(DOCTEST_INCLUDE_DIR "doctest/doctest.h"
    PATHS "C:/msys64/mingw64/include"
)

if (DOCTEST_INCLUDE_DIR)
    include_directories(${DOCTEST_INCLUDE_DIR})
    add_executable(test_banking_system tests/test_banking_system.cpp)
    target_link_libraries(test_banking_system PRIVATE banking_core)
    add_test(NAME TestBankingSystem COMMAND test_banking_system)
endif()
