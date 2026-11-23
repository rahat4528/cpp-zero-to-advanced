cmake_minimum_required(VERSION 3.20)
set(CMAKE_POLICY_VERSION_MINIMUM 3.5 CACHE STRING "" FORCE)

project(OCRScannerTool LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if (MSVC)
    add_compile_options(/W4 /permissive-)
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

include_directories(include)

add_library(ocr_core STATIC
    src/ImageRaster.cpp
    src/OCREngine.cpp
)

add_executable(OCRScanner src/main.cpp)
target_link_libraries(OCRScanner PRIVATE ocr_core)

enable_testing()

find_path(DOCTEST_INCLUDE_DIR "doctest/doctest.h"
    PATHS "C:/msys64/mingw64/include"
)

if (DOCTEST_INCLUDE_DIR)
    include_directories(${DOCTEST_INCLUDE_DIR})
    add_executable(test_ocr_scanner tests/test_ocr_scanner.cpp)
    target_link_libraries(test_ocr_scanner PRIVATE ocr_core)
    add_test(NAME TestOCRScanner COMMAND test_ocr_scanner)
endif()
