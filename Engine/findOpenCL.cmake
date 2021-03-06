
set( OPENCL_SEARCH_PATHS	
	/include
	/lib
	# WINDOWS NVIDIA 
	$ENV{NVSDKCOMPUTE_ROOT}/OpenCL/common
	$ENV{NVSDKCOMPUTE_ROOT}/OpenCL/common/inc
	)

## For 64 bit application
# set( OPENCL_SEARCH_PATHS ${OPENCL_SEARCH_PATHS} $ENV{NVSDKCOMPUTE_ROOT}/OpenCL/common/lib/x64 )
## For 32 bit application
set( OPENCL_SEARCH_PATHS ${OPENCL_SEARCH_PATHS} $ENV{NVSDKCOMPUTE_ROOT}/OpenCL/common/lib/Win32 )

FIND_PATH( OPENCL_INCLUDE_DIR CL/opencl.h PATHS ${OPENCL_SEARCH_PATHS} )
FIND_LIBRARY( OPENCL_LIBRARY OpenCL PATHS ${OPENCL_SEARCH_PATHS} )

# IF( OPENCL_INCLUDE_DIR AND OPENCL_LIBRARY )
# 	SET( OPENCL_FOUND TRUE )
# ENDIF( OPENCL_LIBRARY AND OPENCL_LIBRARY )

include(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(OPENCL DEFAULT_MSG OPENCL_INCLUDE_DIR OPENCL_LIBRARY)

## message for OPENCL
IF( OPENCL_FOUND )
	MESSAGE (" Found OPENCL ")
ELSE( OPENCL_FOUND )
	MESSAGE( FATAL_ERROR "Could not find OPENCL!" )
ENDIF( OPENCL_FOUND )

