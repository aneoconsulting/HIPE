cmake_minimum_required (VERSION 3.7.1)
project (Hipe)

# Configure external libraries.
include("${CMAKE_SOURCE_DIR}/cmake/hipe_external_config.cmake")

# Options
option(USE_GPERFTOOLS "Use gperftools." OFF)

#controls where to build the executables after "make"
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/target)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/target/libs)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/target/libs)

add_subdirectory(source)
add_subdirectory(tests)
