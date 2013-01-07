set( PNG_SEARCH_PATHS
	${PNG_PATH}
	${PNG_PATH}/include
	${PNG_PATH}/lib
	${PNG_PATH}/bin
	${ZLIB_PATH}
	${ZLIB_PATH}/include
	${ZLIB_PATH}/lib
	${ZLIB_PATH}/bin
	)

find_path( PNG_INCLUDE_DIR png.h PATHS ${PNG_SEARCH_PATHS} )
find_path( ZLIB_INCLUDE_DIR zlib.h PATHS ${PNG_SEARCH_PATHS} )
find_library( PNG_LIBRARY libpng PATHS ${PNG_SEARCH_PATHS} )
find_library( ZLIB_LIBRARY NAMES zlib libz PATHS ${PNG_SEARCH_PATHS} )

find_file( PNG_DLL libpng13.dll PATHS ${PNG_SEARCH_PATHS} )
find_file( ZLIB_DLL zlib1.dll PATHS ${PNG_SEARCH_PATHS} )

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(LIBPNG 
	"Could not find  LIBPNG! Please provide the path via\n-DPNG_PATH=\"path\"\n" 
	PNG_INCLUDE_DIR PNG_LIBRARY PNG_DLL)
find_package_handle_standard_args(ZLIB 
	"Could not find  ZLIB! Please provide the path via\n-DZLIB_PATH=\"path\"\n" 
	ZLIB_INCLUDE_DIR ZLIB_LIBRARY ZLIB_DLL)

mark_as_advanced( PNG_PATH PNG_INCLUDE_DIR PNG_LIBRARY PNG_DLL )
mark_as_advanced( ZLIB_PATH ZLIB_INCLUDE_DIR ZLIB_LIBRARY ZLIB_DLL )
