cmake_minimum_required(VERSION 3.20)
set(CMAKE_POLICY_VERSION_MINIMUM 3.5 CACHE STRING "" FORCE)

project(YouTubeVideoDownloader LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if (MSVC)
    add_compile_options(/W4 /permissive-)
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

include_directories(include)

add_library(downloader_core STATIC
    src/ChunkDownloader.cpp
    src/MockMediaServer.cpp
)

if (WIN32)
    target_link_libraries(downloader_core PUBLIC wininet)
endif()

add_executable(VideoDownloader src/main.cpp)
target_link_libraries(VideoDownloader PRIVATE downloader_core)

enable_testing()

find_path(DOCTEST_INCLUDE_DIR "doctest/doctest.h"
    PATHS "C:/msys64/mingw64/include"
)

if (DOCTEST_INCLUDE_DIR)
    include_directories(${DOCTEST_INCLUDE_DIR})
    add_executable(test_video_downloader tests/test_video_downloader.cpp)
    target_link_libraries(test_video_downloader PRIVATE downloader_core)
    add_test(NAME TestVideoDownloader COMMAND test_video_downloader)
endif()
