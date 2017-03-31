cmake_minimum_required (VERSION 3.7.1)

include("${CMAKE_SOURCE_DIR}/OwnerConfigPath.cmake")


add_subdirectory(core)
add_subdirectory(filter)
add_subdirectory(streaming)
add_subdirectory(http)
add_subdirectory(hype_server)
add_subdirectory(orchestrator)
add_subdirectory(json)


add_dependencies(filter core)
add_dependencies(filter streaming)
add_dependencies(http filter)

add_dependencies(orchestrator core filter http json)
add_dependencies(hype_server core http orchestrator json streaming)


