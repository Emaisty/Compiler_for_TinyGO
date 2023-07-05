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
        ${HEADERS}/AST.h
        ${HEADERS}/parser.h

        ${SOURCES}/lexer.cpp
        ${SOURCES}/AST.cpp
        ${SOURCES}/parser.cpp

        main.cpp)
add_executable(compiler ${SOURCE_FILES})
