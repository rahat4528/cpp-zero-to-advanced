cmake_minimum_required(VERSION 3.20)
set(CMAKE_POLICY_VERSION_MINIMUM 3.5 CACHE STRING "" FORCE)

project(CustomIDEPlugin LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if (MSVC)
    add_compile_options(/W4 /permissive-)
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

include_directories(include)

# Plugin Host Core Library
add_library(plugin_host_core STATIC
    src/PluginManager.cpp
)

if (UNIX AND NOT APPLE)
    target_link_libraries(plugin_host_core PUBLIC dl)
endif()

# Sample Dynamic Plugin
add_library(FormatterPlugin SHARED
    plugins/FormatterPlugin.cpp
)
set_target_properties(FormatterPlugin PROPERTIES
    PREFIX "lib"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/plugins"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/plugins"
)

# IDE Host Executable
add_executable(CustomIDEHost src/main.cpp)
target_link_libraries(CustomIDEHost PRIVATE plugin_host_core)

enable_testing()

find_path(DOCTEST_INCLUDE_DIR "doctest/doctest.h"
    PATHS "C:/msys64/mingw64/include"
)

if (DOCTEST_INCLUDE_DIR)
    include_directories(${DOCTEST_INCLUDE_DIR})
    add_executable(test_plugin_system tests/test_plugin_system.cpp)
    target_link_libraries(test_plugin_system PRIVATE plugin_host_core)
    add_test(NAME TestPluginSystem COMMAND test_plugin_system)
endif()
