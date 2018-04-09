# These settings are only required at the globel scope but they are collected
# here to enable inclusion in a nested stand-alone scope, e.g. for the
# compilation of a single HIPE library.

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wall")
if (UNIX)
	include_directories("${CMAKE_SOURCE_DIR}/header" "/usr/include" "/usr/local/include")
else(WIN32)
	include_directories("${CMAKE_SOURCE_DIR}/header")
endif()
include("${CMAKE_SOURCE_DIR}/cmake/hipe_external_config.cmake")
