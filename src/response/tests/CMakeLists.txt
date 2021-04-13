cmake_minimum_required(VERSION 3.19)
project(HttpResponse)

set(CMAKE_CXX_STANDARD 20)

# gTest ------------------------------------------------------------------------

# This module enables populating content at configure time
include(FetchContent)
FetchContent_Declare(gtest
        QUIET
        URL https://github.com/google/googletest/archive/release-1.10.0.tar.gz
        )
FetchContent_MakeAvailable(gtest)

# ------------------------------------------------------------------------------

# EXECUTABLE -------------------------------------------------------------------

add_executable(${PROJECT_NAME}
        main.cpp
        ../src/response.cpp)

target_include_directories(${PROJECT_NAME} PUBLIC ../include)

target_link_libraries(${PROJECT_NAME}
        PRIVATE
        gtest
        gtest_main)

# ------------------------------------------------------------------------------
