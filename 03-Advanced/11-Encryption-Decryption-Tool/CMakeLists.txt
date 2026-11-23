cmake_minimum_required(VERSION 3.20)
set(CMAKE_POLICY_VERSION_MINIMUM 3.5 CACHE STRING "" FORCE)

project(EncryptionDecryptionTool LANGUAGES CXX)

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

add_library(aes_gcm_core STATIC
    src/AESGCMCipher.cpp
)
target_link_libraries(aes_gcm_core PUBLIC OpenSSL::Crypto)

add_executable(EncryptionTool src/main.cpp)
target_link_libraries(EncryptionTool PRIVATE aes_gcm_core)

enable_testing()

find_path(DOCTEST_INCLUDE_DIR "doctest/doctest.h"
    PATHS "C:/msys64/mingw64/include"
)

if (DOCTEST_INCLUDE_DIR)
    include_directories(${DOCTEST_INCLUDE_DIR})
    add_executable(test_encryption_tool tests/test_encryption_tool.cpp)
    target_link_libraries(test_encryption_tool PRIVATE aes_gcm_core)
    add_test(NAME TestEncryptionTool COMMAND test_encryption_tool)
endif()
