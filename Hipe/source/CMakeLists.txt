cmake_minimum_required (VERSION 3.7.1)
project (Hype)

##SET( CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -fpermissive" ) ## might be needed in the future
include("${CMAKE_SOURCE_DIR}/boost_config.cmake")

add_subdirectory(core)
add_subdirectory(filter)
add_subdirectory(http)
add_subdirectory(hype_server)
add_subdirectory(orchestrator)
add_subdirectory(json)

add_dependencies(filter core)
add_dependencies(http filter)
add_dependencies(orchestrator core filter http json)
add_dependencies(hype_server http orchestrator json)


