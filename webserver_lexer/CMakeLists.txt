cmake_minimum_required(VERSION 3.17)
project(webserver_lexer)

set(CMAKE_CXX_STANDARD 20)

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fno-omit-frame-pointer -fsanitize=address")
set(CMAKE_LINKER_FLAGS_DEBUG "${CMAKE_LINKER_FLAGS_DEBUG} -fno-omit-frame-pointer -fsanitize=address")


# LIBRARY ----------------------------------------------------------------------
add_library(${PROJECT_NAME}_lib
        lib_include/lexer.hpp
        lib_src/lexer.cpp
        lib_include/token.hpp
        lib_src/token.cpp
        )

#target_include_directories(${PROJECT_NAME}_lib
#        PUBLIC
#        ${PROJECT_SOURCE_DIR}/lib_src
#        )
# ------------------------------------------------------------------------------

# gTest ------------------------------------------------------------------------

#include_directories(${gtest_SOURECE_DIR}/include ${gtest_SOURCE_DIR})
#add_subdirectory(googletest)

# ------------------------------------------------------------------------------

# EXECUTABLE -------------------------------------------------------------------
#add_executable(${PROJECT_NAME} tests/main.cpp)
#
#target_link_libraries(${PROJECT_NAME}
#        PRIVATE
#        ${PROJECT_NAME}_lib
#        gtest
#        gtest_main
#        )
# ------------------------------------------------------------------------------
