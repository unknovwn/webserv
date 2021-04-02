cmake_minimum_required(VERSION 3.17)
project(webserver_lexer)

set(CMAKE_CXX_STANDARD 20)

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fno-omit-frame-pointer -fsanitize=address")
set(CMAKE_LINKER_FLAGS_DEBUG "${CMAKE_LINKER_FLAGS_DEBUG} -fno-omit-frame-pointer -fsanitize=address")


# LIBRARY ----------------------------------------------------------------------

include_directories(../include)

add_library(${PROJECT_NAME}_lib
        ../include/lexer.hpp
        ../src/lexer.cpp
        ../include/token.hpp
        ../src/token.cpp
        )

target_include_directories(${PROJECT_NAME}_lib
        PUBLIC
        ${PROJECT_SOURCE_DIR}/src
        )
# ------------------------------------------------------------------------------

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
add_executable(${PROJECT_NAME} ../tests/lexer_test.cpp)

target_link_libraries(${PROJECT_NAME}
        PRIVATE
        ${PROJECT_NAME}_lib
        gtest
        gtest_main
        )
# ------------------------------------------------------------------------------
