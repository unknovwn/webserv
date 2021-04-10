cmake_minimum_required(VERSION 3.17)
project(request_parser)

# LIBRARY ----------------------------------------------------------------------

set(CMAKE_CXX_STANDARD 20)

add_library(${PROJECT_NAME}
        src/request_parser.cpp
        src/request.cpp)

target_include_directories(${PROJECT_NAME} PUBLIC include)

# ------------------------------------------------------------------------------
