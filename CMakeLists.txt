
cmake_minimum_required(VERSION 3.10)

project(lingeling)

set (liblgl_SRC lglib.c lglbnr.c lgldimacs.c lglopts.c)

add_library(lgl ${liblgl_SRC})

file(READ "${PROJECT_SOURCE_DIR}/VERSION" LGL_VERSION)
string(REGEX REPLACE "\n$" "" LGL_VERSION "${LGL_VERSION}")
configure_file("${PROJECT_SOURCE_DIR}/lglcfg.h.in"
	       "${PROJECT_BINARY_DIR}/lglcfg.h")
configure_file("${PROJECT_SOURCE_DIR}/lglcflags.h.in"
	       "${PROJECT_BINARY_DIR}/lglcflags.h")

include_directories("${PROJECT_BINARY_DIR}")     

add_definitions(-DNLGLOG -DNDEBUG -DNCHKSOL -DNLGLDRUPLIG -DNLGLYALSAT)
add_definitions(-DNLGLFILES -DNLGLDEMA)


