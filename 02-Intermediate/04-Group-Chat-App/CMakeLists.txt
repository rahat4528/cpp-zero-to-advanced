cmake_minimum_required(VERSION 3.20)
set(CMAKE_POLICY_VERSION_MINIMUM 3.5 CACHE STRING "" FORCE)

project(GroupChatApp VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(MSVC)
    add_compile_options(/W4 /permissive-)
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

find_package(Threads REQUIRED)

add_library(chat_core STATIC
    src/ChatServer.cpp
    src/ChatClient.cpp
)
target_include_directories(chat_core PUBLIC
    include
)
target_link_libraries(chat_core PUBLIC Threads::Threads)
if(WIN32)
    target_link_libraries(chat_core PUBLIC ws2_32)
endif()

add_executable(GroupChatServer
    src/server_main.cpp
)
target_link_libraries(GroupChatServer PRIVATE chat_core)

add_executable(GroupChatClient
    src/client_main.cpp
)
target_link_libraries(GroupChatClient PRIVATE chat_core)

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

add_executable(GroupChatApp_tests
    tests/test_chat_protocol.cpp
)
target_link_libraries(GroupChatApp_tests PRIVATE chat_core doctest::doctest)

add_test(NAME GroupChatProtocolUnitTests COMMAND GroupChatApp_tests)
