cmake_minimum_required(VERSION 3.20)
set(CMAKE_POLICY_VERSION_MINIMUM 3.5 CACHE STRING "" FORCE)

project(FaceLoginAuthentication LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if (MSVC)
    add_compile_options(/W4 /permissive-)
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

include_directories(include)

add_library(face_auth_core STATIC
    src/FaceDescriptor.cpp
    src/FaceAuthManager.cpp
)

add_executable(FaceLoginAuth src/main.cpp)
target_link_libraries(FaceLoginAuth PRIVATE face_auth_core)

enable_testing()

find_path(DOCTEST_INCLUDE_DIR "doctest/doctest.h"
    PATHS "C:/msys64/mingw64/include"
)

if (DOCTEST_INCLUDE_DIR)
    include_directories(${DOCTEST_INCLUDE_DIR})
    add_executable(test_face_auth tests/test_face_auth.cpp)
    target_link_libraries(test_face_auth PRIVATE face_auth_core)
    add_test(NAME TestFaceAuth COMMAND test_face_auth)
endif()
