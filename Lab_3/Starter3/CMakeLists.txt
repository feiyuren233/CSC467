cmake_minimum_required(VERSION 3.2)
project(lab3)

set(CMAKE_C_COMPILER g++)
set(CMAKE_CXX_FLAGS "-g -O0 -Wall -std=c++11")
set(CMAKE_BUILD_DIR ${CMAKE_SOURCE_DIR}/build)

add_custom_command(
        OUTPUT ${CMAKE_SOURCE_DIR}/y.tab.c
        COMMAND bash -c "bison -dtvy parser.y")

add_custom_command(
        OUTPUT ${CMAKE_SOURCE_DIR}/y.tab.h
        COMMAND bash -c "bison -dtvy parser.y")

add_custom_command(
	OUTPUT ${CMAKE_SOURCE_DIR}/parser.c
	COMMAND bash -c "mv -f y.tab.c parser.c"
	DEPENDS y.tab.c)

add_custom_command(
	OUTPUT ${CMAKE_SOURCE_DIR}/parser.tab.h
	COMMAND bash -c "mv -f y.tab.h parser.tab.h"
	DEPENDS y.tab.h)

add_custom_command(
        OUTPUT ${CMAKE_SOURCE_DIR}/scanner.c
        COMMAND bash -c "flex -t scanner.l >scanner.c")

add_custom_target(run ALL DEPENDS parser.c parser.tab.h scanner.c)

set(SOURCE_FILES compiler467.c globalvars.c scanner.c y.tab.c ast.cpp semantic.cpp symbol.cpp)

add_executable(compiler467 ${SOURCE_FILES})

