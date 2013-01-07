cmake_minimum_required (VERSION 2.8)
project(ObjLoader)
set(PROJ_NAME ObjLoader)

if(WIN32)
add_definitions(-D_CRT_SECURE_NO_WARNINGS)
endif(WIN32)

###################
## CONFIGURATION ##
###################
set(VERSION_MAJOR 0)
set(VERSION_MINOR 5)

configure_file ( "config.h.in" "${CMAKE_CURRENT_SOURCE_DIR}/${PROJ_NAME}_config.h" )
###################

set( CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR})

set( OUTPUT_NAME ${PROJ_NAME})
set( OUTPUT_NAME_Debug ${PROJ_NAME}_D)

set( SRC_FILES objloader.h objloader.cpp)

add_library(${PROJ_NAME} ${SRC_FILES})

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${ENGINE_DIR})
