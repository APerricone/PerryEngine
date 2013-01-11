cmake_minimum_required (VERSION 2.8)
project(Engine)
set(PROJ_NAME Engine)

if(WIN32)
add_definitions(-D_CRT_SECURE_NO_WARNINGS)
endif(WIN32)

###################
## CONFIGURATION ##
###################
set(VERSION_MAJOR 0)
set(VERSION_MINOR 4)

add_definitions(-D_EXPORT)

configure_file ( "config.h.in" "${CMAKE_CURRENT_SOURCE_DIR}/${PROJ_NAME}_config.h" )
###################

set( CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR})

find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)

set( ENGINE_DIR ${CMAKE_CURRENT_SOURCE_DIR} PARENT_SCOPE)
include_directories(${ENGINE_DIR})
include_directories(${OPENGL_INCLUDE_DIR})
include_directories(${GLEW_INCLUDE_DIR})

set( LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/ExeDir)
set( OUTPUT_NAME ${PROJ_NAME})
set( OUTPUT_NAME_Debug ${PROJ_NAME}_D)

set( SRC_FILES mymacros.h opengl.h ${PROJ_NAME}_config.h)
set( SRC_FILES ${SRC_FILES} vector.h sphere.h)
set( SRC_FILES ${SRC_FILES} ray.h ray.inl)
set( SRC_FILES ${SRC_FILES} camera.h camera.cpp)
set( SRC_FILES ${SRC_FILES} dynamicmesh.h dynamicmesh.cpp)
set( SRC_FILES ${SRC_FILES} mesh.h mesh.cpp)
set( SRC_FILES ${SRC_FILES} image.h image.cpp)
set( SRC_FILES ${SRC_FILES} matrix.h matrix.inl)
set( SRC_FILES ${SRC_FILES} material.h material.cpp)
set( SRC_FILES ${SRC_FILES} light.h light.cpp)
set( SRC_FILES ${SRC_FILES} mrt.h mrt.cpp)
set( SRC_FILES ${SRC_FILES} glsl.h glsl.cpp)
set( SRC_FILES ${SRC_FILES} fullscreenblit.h fullscreenblit.cpp)
set( SRC_FILES ${SRC_FILES} glperfcounter.h glperfcounter.cpp)
set( SRC_FILES ${SRC_FILES} fxaa_interface.h fxaa_interface.cpp)
set( SRC_FILES ${SRC_FILES} scene.h scene.cpp)
set( SRC_FILES ${SRC_FILES} gamma.h gamma.cpp)
set( SRC_FILES ${SRC_FILES} bestfitcube.h bestfitcube.cpp)
set( SRC_FILES ${SRC_FILES} node.h node.cpp)
set( SRC_FILES ${SRC_FILES} model.h model.cpp)
set( SRC_FILES ${SRC_FILES} collisiontriangle.h collisiontriangle.cpp)
set( SRC_FILES ${SRC_FILES} ssao.h ssao.cpp)
set( SRC_FILES ${SRC_FILES} miniball.h)
set( SRC_FILES ${SRC_FILES} log.h log.cpp)
set( SRC_FILES ${SRC_FILES} font.h)

set( SHADERS_PATH ${LIBRARY_OUTPUT_DIRECTORY} )
set( SHADERS_FILES ${SHADERS_FILES} ${SHADERS_PATH}/blit.vert ${SHADERS_PATH}/blit.frag)
set( SHADERS_FILES ${SHADERS_FILES} ${SHADERS_PATH}/material.vert ${SHADERS_PATH}/material.frag)
set( SHADERS_FILES ${SHADERS_FILES} ${SHADERS_PATH}/fxaa.frag)
set( SHADERS_FILES ${SHADERS_FILES} ${SHADERS_PATH}/ssao.frag)
set( SHADERS_FILES ${SHADERS_FILES} ${SHADERS_PATH}/gamma.frag)
set( SRC_FILES ${SRC_FILES} ${SHADERS_FILES})
source_group(Shaders FILES ${SHADERS_FILES})

set( SHADERS_LIGHT ${SHADERS_PATH}/light.start.frag)
set( SHADERS_LIGHT ${SHADERS_LIGHT} ${SHADERS_PATH}/light.hard.frag)
set( SHADERS_LIGHT ${SHADERS_LIGHT} ${SHADERS_PATH}/light.pcf.frag)
set( SHADERS_LIGHT ${SHADERS_LIGHT} ${SHADERS_PATH}/light.pcss.frag)
set( SHADERS_LIGHT ${SHADERS_LIGHT} ${SHADERS_PATH}/light.final.frag)
set( SHADERS_LIGHT ${SHADERS_LIGHT} ${SHADERS_PATH}/light.specular.frag)
source_group(Shaders\\light FILES ${SHADERS_LIGHT})
set( SRC_FILES ${SRC_FILES} ${SHADERS_LIGHT})

set( SHADERS_SRCS fxaa.frag Fxaa3_11.h)
source_group(Shaders\\src FILES ${SHADERS_SRCS})
set( SRC_FILES ${SRC_FILES} ${SHADERS_SRCS})

add_library(${PROJ_NAME} ${SRC_FILES})

target_link_libraries(${PROJ_NAME} ${OPENGL_gl_LIBRARY})
target_link_libraries(${PROJ_NAME} ${OPENGL_glu_LIBRARY})

#copy dll in exeDir
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

	my_copy( ${GLEW_DLL} ${SHADERS_PATH})

endif(WIN32)

#proccess fxaa
file(TO_NATIVE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/fxaa.frag FXAA_SRC)
file(TO_NATIVE_PATH ${RUNTIME_OUTPUT_DIRECTORY}/fxaa.frag FXAA_DEST)

if(WIN32)
	add_custom_command(
		TARGET ${PROJ_NAME}
		POST_BUILD
		COMMAND "${CMAKE_CXX_COMPILER}"
		ARGS /EP ${FXAA_SRC} > ${FXAA_DEST}
		MAIN_DEPENDENCY ${FXAA_SRC}
		DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/Fxaa3_11.h
		WORKING_DIRECTORY .
	)
else(WIN32)
	message( "NEED CHECK" )
	add_custom_command(
		TARGET ${PROJ_NAME}
		POST_BUILD
		COMMAND "${CMAKE_CXX_COMPILER}"
		ARGS -E ${FXAA_SRC} > ${FXAA_DEST}
		MAIN_DEPENDENCY fxaa.frag
		DEPENDS Fxaa3_8.h
		WORKING_DIRECTORY .
	)
endif(WIN32)
