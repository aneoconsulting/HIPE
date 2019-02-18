# These settings are only required at the globel scope but they are collected
# here to enable inclusion in a nested stand-alone scope, e.g. for the
# compilation of a single HIPE library.

if (UNIX)
  include_directories("${CMAKE_SOURCE_DIR}/header" "/usr/include" "/usr/local/include")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=gnu++11 -Wall -Wno-unknown-pragmas")
else(WIN32)
	include_directories("${CMAKE_SOURCE_DIR}/header")
endif()

message(status "Configure HIPE_EXTERNAL : ${CMAKE_CURRENT_LIST_DIR}/hipe_external_config.cmake")
include("${CMAKE_CURRENT_LIST_DIR}/hipe_external_config.cmake")
