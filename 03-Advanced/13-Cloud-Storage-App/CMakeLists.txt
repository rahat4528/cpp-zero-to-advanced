cmake_minimum_required(VERSION 3.20)
set(CMAKE_POLICY_VERSION_MINIMUM 3.5 CACHE STRING "" FORCE)

project(CloudStorageApp LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if (MSVC)
    add_compile_options(/W4 /permissive-)
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

find_package(OpenSSL REQUIRED)

include_directories(include)

add_library(cloud_core STATIC
    src/CloudServer.cpp
    src/CloudClient.cpp
)
target_link_libraries(cloud_core PUBLIC OpenSSL::Crypto)

if (WIN32)
    target_link_libraries(cloud_core PUBLIC ws2_32)
endif()

add_executable(CloudServer src/server_main.cpp)
target_link_libraries(CloudServer PRIVATE cloud_core)

add_executable(CloudClient src/client_main.cpp)
target_link_libraries(CloudClient PRIVATE cloud_core)

enable_testing()

find_path(DOCTEST_INCLUDE_DIR "doctest/doctest.h"
    PATHS "C:/msys64/mingw64/include"
)

if (DOCTEST_INCLUDE_DIR)
    include_directories(${DOCTEST_INCLUDE_DIR})
    add_executable(test_cloud_storage tests/test_cloud_storage.cpp)
    target_link_libraries(test_cloud_storage PRIVATE cloud_core)
    add_test(NAME TestCloudStorage COMMAND test_cloud_storage)
endif()
