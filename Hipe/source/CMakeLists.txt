cmake_minimum_required (VERSION 3.7.1)
project (Hype)

include("${CMAKE_SOURCE_DIR}/boost_config.cmake")

add_subdirectory(exception)
add_subdirectory(filter)
add_subdirectory(http)
add_subdirectory(hype_server)

add_dependencies(filter exception)
add_dependencies(http filter)
add_dependencies(hype_server http)


