cmake_minimum_required (VERSION 3.7.1)

# add_subdirectory inherits parent scope so there is no need to re-include this.
# include("${CMAKE_SOURCE_DIR}/cmake/hipe_external_config.cmake")


if(USE_DLIB)
  add_definitions(-DUSE_DLIB)
  message(STATUS "Dlib : ON")
else()
  message(STATUS "Dlib : OFF")
endif()


if (WIN32)
  add_compile_options(/wd4820 /wd4668 /wd4619 /wd4626 /wd5031 /wd4365 /wd4710)
endif()

if(UNIX)
  message(STATUS "UNIX IS DETECTED")
  if(USE_GPERFTOOLS)
    # TODO
    # Make this optional and maybe move it.
    include_directories(${GPERFTOOLS_DIR}/include)
    add_definitions(-DUSE_GPERFTOOLS)
    message(STATUS "Google perftools : ON")
  endif(USE_GPERFTOOLS)
endif(UNIX)

add_subdirectory(core)
add_subdirectory(streaming_rtsp)
# add_subdirectory(streaming)
add_subdirectory(data)
add_subdirectory(pydata)
add_subdirectory(corefilter)
add_subdirectory(filter)
add_subdirectory(json)
add_subdirectory(orchestrator)
add_subdirectory(http)
add_subdirectory(hipe_server)

# TODO
# Determine if this is necessary. CMake should detect the dependencies from the
# target_link_library commands.

add_dependencies(pydata data)
add_dependencies(pydata data)
add_dependencies(data core)
add_dependencies(filter core corefilter)
add_dependencies(filter pydata)
#add_dependencies(filter streaming)
add_dependencies(filter data)
#add_dependencies(streaming streaming_rtsp)

add_dependencies(http filter data json orchestrator)

add_dependencies(orchestrator core filter json data)
add_dependencies(hipe_server core http orchestrator json)
