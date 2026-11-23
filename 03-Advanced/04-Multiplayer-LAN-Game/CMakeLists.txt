cmake_minimum_required(VERSION 3.20)
set(CMAKE_POLICY_VERSION_MINIMUM 3.5 CACHE STRING "" FORCE)

project(MultiplayerLANGame LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if (MSVC)
    add_compile_options(/W4 /permissive-)
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

include_directories(include)

add_library(lan_game_core STATIC
    src/AuthoritativeServer.cpp
    src/GameClient.cpp
)

if (WIN32)
    target_link_libraries(lan_game_core PUBLIC ws2_32)
endif()

add_executable(MultiplayerServer src/server_main.cpp)
target_link_libraries(MultiplayerServer PRIVATE lan_game_core)

add_executable(MultiplayerClient src/client_main.cpp)
target_link_libraries(MultiplayerClient PRIVATE lan_game_core)

enable_testing()

find_path(DOCTEST_INCLUDE_DIR "doctest/doctest.h"
    PATHS "C:/msys64/mingw64/include"
)

if (DOCTEST_INCLUDE_DIR)
    include_directories(${DOCTEST_INCLUDE_DIR})
    add_executable(test_multiplayer_game tests/test_multiplayer_game.cpp)
    target_link_libraries(test_multiplayer_game PRIVATE lan_game_core)
    add_test(NAME TestMultiplayerLANGame COMMAND test_multiplayer_game)
endif()
