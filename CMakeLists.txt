cmake_minimum_required(VERSION 3.21)
project(compiler)

set(CMAKE_CXX_STANDARD 20)

find_package(magic_enum CONFIG REQUIRED)

set(FRONDEND_HEADERS src/FrontEnd/Headers)
set(FRONDEND_SOURCES src/FrontEnd/Sources)

set(MIDDLEEND_HEADERS src/MiddleEnd/Headers)
set(MIDDLEEND_SOURCES src/MiddleEnd/Sources)

set(BACKEND_HEADERS src/BackEnd/Headers)
set(BACKEND_SOURCES src/BackEnd/Sources)

set(COMMON_HEADERS src/Common/Headers)
set(COMMON_SOURCES src/Common/Sources)

include_directories(${COMMON_HEADERS})
include_directories(${FRONDEND_HEADERS})
include_directories(${MIDDLEEND_HEADERS})
include_directories(${BACKEND_HEADERS})

set(
        SOURCE_FILES

        ${COMMON_SOURCES}/types.cpp

        ${FRONDEND_SOURCES}/lexer.cpp
        ${FRONDEND_SOURCES}/AST_parser.cpp
        ${FRONDEND_SOURCES}/AST_checker.cpp
        ${FRONDEND_SOURCES}/AST_IR.cpp
        ${FRONDEND_SOURCES}/parser.cpp

        ${MIDDLEEND_SOURCES}/IR.cpp
        ${MIDDLEEND_SOURCES}/IR_codegen.cpp

        ${BACKEND_SOURCES}/Operands.cpp
        ${BACKEND_SOURCES}/T86Inst.cpp

        main.cpp)
add_executable(compiler ${SOURCE_FILES})

target_link_libraries(compiler PRIVATE magic_enum::magic_enum)