set( GLEW_SEARCH_PATHS
	${GLEW_PATH}
	${GLEW_PATH}/include
	${GLEW_PATH}/lib
	${GLEW_PATH}/bin
	)

if(WIN32)
	if(MINGW)
		set( GLEW_SEARCH_PATHS ${GLEW_SEARCH_PATHS}
			${GLEW_PATH}/lib-mingw)
	endif(MINGW)

	if(CYGWIN)
		set( GLEW_SEARCH_PATHS ${GLEW_SEARCH_PATHS}
			${GLEW_PATH}/lib-cygwin)
	endif(CYGWIN)

	find_file( GLEW_DLL glew32.dll PATHS ${GLEW_SEARCH_PATHS} )
endif (WIN32)

find_path( GLEW_INCLUDE_DIR GL/glew.h PATHS ${GLEW_SEARCH_PATHS} )
find_library( GLEW_LIBRARY glew32 PATHS ${GLEW_SEARCH_PATHS} )

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(GLEW 
	"Could not find GLEW! Please provide the path via \n-DGLEW_PATH=\"path\"" 
	GLEW_INCLUDE_DIR GLEW_LIBRARY GLEW_DLL)

mark_as_advanced( GLEW_PATH GLEW_INCLUDE_DIR GLEW_LIBRARY GLEW_DLL )
