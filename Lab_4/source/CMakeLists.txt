cmake_minimum_required(VERSION 3.2)
project(lab3)

set(CMAKE_C_COMPILER g++)
set(CMAKE_C_FLAGS "-g -O0 -Wall")

find_package(BISON)
find_package(FLEX)

BISON_TARGET(BisonParser ${CMAKE_SOURCE_DIR}/parser.y ${CMAKE_SOURCE_DIR}/parser.c)
FLEX_TARGET(FlexScanner ${CMAKE_SOURCE_DIR}/scanner.l ${CMAKE_SOURCE_DIR}/scanner.c)
ADD_FLEX_BISON_DEPENDENCY(FlexScanner BisonParser)

set(SOURCE_FILES compiler467.c globalvars.c ast.cpp semantic.cpp symbol.cpp codegen.cpp arb.cpp arb.hpp)

add_executable(compiler467 ${SOURCE_FILES} 
			   ${BISON_BisonParser_OUTPUTS} ${FLEX_FlexScanner_OUTPUTS})


