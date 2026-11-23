cmake_minimum_required(VERSION 3.20)
set(CMAKE_POLICY_VERSION_MINIMUM 3.5 CACHE STRING "" FORCE)

project(NotesSaverApp VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(MSVC)
    add_compile_options(/W4 /permissive-)
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

find_package(SQLite3 REQUIRED)

add_library(notes_core STATIC
    src/NotesDatabase.cpp
)
target_include_directories(notes_core PUBLIC
    include
)
target_link_libraries(notes_core PUBLIC SQLite3::SQLite3)

add_executable(NotesSaver
    src/main.cpp
)
target_link_libraries(NotesSaver PRIVATE notes_core)

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

add_executable(NotesSaver_unit_tests
    tests/test_notes.cpp
)
target_link_libraries(NotesSaver_unit_tests PRIVATE notes_core doctest::doctest)

add_test(NAME NotesSaverUnitTests COMMAND NotesSaver_unit_tests)
