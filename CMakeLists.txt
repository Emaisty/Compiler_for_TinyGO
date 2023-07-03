cmake_minimum_required(VERSION 3.25)
project(compiler)

set(CMAKE_CXX_STANDARD 20)

set(HEADERS src/Headers)
set(SOURCES src/Sources)

include_directories(${HEADERS})
include_directories(${SOURCES})


set(
        SOURCE_FILES

        ${HEADERS}/lexer.h

        ${SOURCES}/lexer.cpp

        main.cpp)
add_executable(compiler ${SOURCE_FILES})
