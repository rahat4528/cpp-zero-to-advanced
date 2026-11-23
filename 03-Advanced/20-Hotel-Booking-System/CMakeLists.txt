cmake_minimum_required(VERSION 3.20)
set(CMAKE_POLICY_VERSION_MINIMUM 3.5 CACHE STRING "" FORCE)
project(HotelBookingSystem LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if (MSVC)
    add_compile_options(/W4 /permissive-)
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

find_package(SQLite3 REQUIRED)
find_path(DOCTEST_INCLUDE_DIR "doctest/doctest.h" PATHS "C:/msys64/mingw64/include" "/usr/include" "/usr/local/include")

# Core library
add_library(hotel_core STATIC
    src/HotelDatabase.cpp
    src/HotelManager.cpp
)
target_include_directories(hotel_core PUBLIC
    include
    ${SQLite3_INCLUDE_DIRS}
)
target_link_libraries(hotel_core PUBLIC
    SQLite3::SQLite3
)

# Main reservation CLI application
add_executable(HotelReservation src/main.cpp)
target_link_libraries(HotelReservation PRIVATE hotel_core)

# Test suite
enable_testing()
add_executable(test_hotel_booking tests/test_hotel_booking.cpp)
target_include_directories(test_hotel_booking PRIVATE ${DOCTEST_INCLUDE_DIR})
target_link_libraries(test_hotel_booking PRIVATE hotel_core)
add_test(NAME test_hotel_booking COMMAND test_hotel_booking)
