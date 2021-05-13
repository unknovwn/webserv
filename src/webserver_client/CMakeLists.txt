cmake_minimum_required(VERSION 3.17)
project(webserver_client)
set(CMAKE_CXX_STANDARD 20)

# LIBRARY ----------------------------------------------------------------------

add_library(${PROJECT_NAME}
        src/client.cpp
        src/timer.cpp
        ../request_parser/build/librequest_parser.a)


target_include_directories(${PROJECT_NAME} PUBLIC include)
target_include_directories(${PROJECT_NAME} PUBLIC ../request_parser/include)

# ------------------------------------------------------------------------------
