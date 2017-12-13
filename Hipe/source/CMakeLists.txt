cmake_minimum_required (VERSION 3.7.1)

include("${CMAKE_SOURCE_DIR}/cmake/OwnerConfigPath.cmake")


if (USE_DLIB)
  message(STATUS "UNIX IS DETECTED")
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
  if (USE_GPERFTOOLS)
	include_directories(${GPERFTOOLS_DIR}/include)
	add_definitions(-DUSE_GPERFTOOLS)
	message(STATUS "Google perftools : ON")
  endif()
endif()

add_subdirectory(core)
add_subdirectory(data)
add_subdirectory(pydata)
add_subdirectory(filter)
add_subdirectory(streaming)
add_subdirectory(http)
add_subdirectory(hipe_server)
add_subdirectory(orchestrator)
add_subdirectory(json)
add_subdirectory(streaming_rtsp)



add_dependencies(pydata data)
add_dependencies(pydata data)
add_dependencies(data core)
add_dependencies(filter core)
add_dependencies(filter pydata)
add_dependencies(filter streaming)
add_dependencies(filter data)
add_dependencies(streaming streaming_rtsp)

add_dependencies(http filter data json orchestrator)

add_dependencies(orchestrator core filter json data)
add_dependencies(hipe_server core http orchestrator json streaming)


