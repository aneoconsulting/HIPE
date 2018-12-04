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
  add_compile_options(/wd4820 /wd4668 /wd4619 /wd4626 /wd5031 /wd4365 /wd4710 /wd4521 /wd4061 /wd4625 /wd5026 /wd4251 /wd4514 /wd4503 /wd4711 /wd4100 /wd5027 /wd4264)
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

  add_compile_options(-Wno-unknown-pragmas -DLINUX -DUNIX)
  
if(CMAKE_SYSTEM_PROCESSOR MATCHES "^(powerpc|ppc)64le")
	add_compile_options(-mabi=altivec -maltivec)
endif()

endif(UNIX)

add_subdirectory(pydata)

add_subdirectory(data)

add_subdirectory(filter)




add_dependencies(pydata data)
add_dependencies(filter data)
add_dependencies(filter pydata)


