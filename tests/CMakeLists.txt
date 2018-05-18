cmake_minimum_required (VERSION 3.7.1)
project (Hipe)

##set( CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -fpermissive" ) ## might be needed in the future
include("${CMAKE_SOURCE_DIR}/tests/boost_config_tests.cmake")

add_subdirectory(source)

add_dependencies(unit_test core filter data json http orchestrator )


