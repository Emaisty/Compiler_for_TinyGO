cmake_minimum_required(VERSION 3.21)
project(compiler)

set(CMAKE_CXX_STANDARD 20)

set(FRONDEND_HEADERS src/Frontend/Headers)
set(FRONDEND_SOURCES src/Frontend/Sources)

set(MIDDLEEND_HEADERS src/MiddleEnd/Headers)
set(MIDDLEEND_SOURCES src/MiddleEnd/Sources)

set(COMMON_HEADERS src/Common/Headers)
set(COMMON_SOURCES src/Common/Sources)

include_directories(${COMMON_HEADERS})

include_directories(${FRONDEND_HEADERS})
include_directories(${MIDDLEEND_HEADERS})

set(
        SOURCE_FILES

        ${COMMON_SOURCES}/types.cpp

        ${FRONDEND_SOURCES}/lexer.cpp
        ${FRONDEND_SOURCES}/AST_parser.cpp
        ${FRONDEND_SOURCES}/AST_checker.cpp
#        ${FRONDEND_SOURCES}/AST_IR.cpp
        ${FRONDEND_SOURCES}/parser.cpp

#        ${MIDDLEEND_SOURCES}/IR.cpp

        main.cpp)
add_executable(compiler ${SOURCE_FILES})
