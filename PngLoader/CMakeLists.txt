cmake_minimum_required (VERSION 2.8)
project(PngLoader)
set(PROJ_NAME PngLoader)

if(WIN32)
add_definitions(-D_CRT_SECURE_NO_WARNINGS)
endif(WIN32)

###################
## CONFIGURATION ##
###################
set(VERSION_MAJOR 1)
set(VERSION_MINOR 0)

configure_file ( "config.h.in" "${CMAKE_CURRENT_SOURCE_DIR}/${PROJ_NAME}_config.h" )
###################

set( CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR})
find_package(LibPNG REQUIRED)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${ENGINE_DIR})
include_directories(${ZLIB_INCLUDE_DIR})
include_directories(${PNG_INCLUDE_DIR})

set( OUTPUT_NAME ${PROJ_NAME})
set( OUTPUT_NAME_Debug ${PROJ_NAME}_D)

set( SRC_FILES pngloader.h pngloader.cpp)


add_library(${PROJ_NAME} ${SRC_FILES})
target_link_libraries(${PROJ_NAME} ${ZLIB_LIBRARY} ${PNG_LIBRARY})

if(WIN32)
	function(my_copy FILE_PATH DEST_PATH)
		file(TO_NATIVE_PATH ${FILE_PATH} FILE_PATH_TMP)
		file(TO_NATIVE_PATH ${DEST_PATH} DEST_PATH_TMP)
		add_custom_command(
			TARGET ${PROJ_NAME}
			POST_BUILD
			COMMAND copy ${FILE_PATH_TMP} ${DEST_PATH_TMP}
			MAIN_DEPENDENCY ${FILE_PATH}
			WORKING_DIRECTORY .)
	endfunction(my_copy)

	#my_copy( ${PNG_DLL} ${LIBRARY_OUTPUT_DIRECTORY})
	#my_copy( ${ZLIB_DLL} ${LIBRARY_OUTPUT_DIRECTORY})

endif(WIN32)
