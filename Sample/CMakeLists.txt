cmake_minimum_required (VERSION 2.8)
project(Sample)
set(PROJ_NAME Sample)

###################
## CONFIGURATION ##
###################
set(VERSION_MAJOR 1)
set(VERSION_MINOR 0)

configure_file( "config.h.in" "${CMAKE_CURRENT_SOURCE_DIR}/config.h" )

if(WIN32)
	add_definitions(-D_CRT_SECURE_NO_WARNINGS)
endif(WIN32)
###################

set( CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR})

include(findGLFW.cmake)

add_subdirectory(${CMAKE_SOURCE_DIR}/../Engine ${CMAKE_SOURCE_DIR}/Engine-build)
add_subdirectory(${CMAKE_SOURCE_DIR}/../ObjLoader ${CMAKE_SOURCE_DIR}/ObjLoader-build)
add_subdirectory(${CMAKE_SOURCE_DIR}/../PngLoader ${CMAKE_SOURCE_DIR}/PngLoader-build)

include_directories(${GLFW_INCLUDE_DIR})
include_directories(${ENGINE_DIR})
include_directories(${CMAKE_SOURCE_DIR}/../ObjLoader)
include_directories(${CMAKE_SOURCE_DIR}/../PngLoader)
include_directories(${GLEW_INCLUDE_DIR})

set( SRC_FILES game.h game.cpp main.cpp config.h)
set( SRC_FILES ${SRC_FILES} keymanager.h keymanager.cpp)
set( SRC_FILES ${SRC_FILES} rotatearoundcamera.h rotatearoundcamera.cpp)
set( SRC_FILES ${SRC_FILES} window.h window.cpp)

if(WIN32)
	set( SRC_FILES ${SRC_FILES} WinFont.cpp)
endif(WIN32)
if(UNIX)
	set( SRC_FILES ${SRC_FILES} X11Font.cpp)
endif(UNIX)

set( SHADERS_PATH ${CMAKE_CURRENT_SOURCE_DIR}/ExeDir)
set( SHADERS_FILES ${SHADERS_FILES} ${SHADERS_PATH}/simplewriter.vert ${SHADERS_PATH}/simplewriter.frag)
source_group(Shaders FILES ${SHADERS_FILES})

add_executable(${PROJ_NAME} ${SRC_FILES} ${SHADERS_FILES})

target_link_libraries(${PROJ_NAME} ${GLFW_LIBRARY})
target_link_libraries(${PROJ_NAME} ${GLEW_LIBRARY})
target_link_libraries(${PROJ_NAME} Engine)
target_link_libraries(${PROJ_NAME} ObjLoader)
target_link_libraries(${PROJ_NAME} PngLoader)

file(TO_NATIVE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/ExeDir EXEDIR)
file(TO_NATIVE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../Engine/ExeDir/*.* SRC_EXEDIR)


EXECUTE_PROCESS(
	COMMAND mkdir ${EXEDIR}
	WORKING_DIRECTORY .)

ADD_CUSTOM_COMMAND(
	TARGET ${PROJ_NAME}
	POST_BUILD
	COMMAND copy ${SRC_EXEDIR} ${EXEDIR}
	MAIN_DEPENDENCY ${FILE_PATH}
	WORKING_DIRECTORY .)
