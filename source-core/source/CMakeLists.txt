cmake_minimum_required (VERSION 3.7.1)


if (WIN32)
  add_compile_options(/wd4820 /wd4668 /wd4619 /wd4626 /wd5031 /wd4365 /wd4710 /wd4514 /wd4503)
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
  add_compile_options("-Wno-unknown-pragmas")

endif(UNIX)

add_subdirectory(core)

add_subdirectory(coredata)

add_subdirectory(corefilter)

add_subdirectory(corebench)

add_subdirectory(json)
add_subdirectory(orchestrator)
add_subdirectory(http)
add_subdirectory(hipe_server)


add_dependencies(corefilter coredata)
add_dependencies(json coredata)

add_dependencies(http coredata corefilter json orchestrator)

add_dependencies(orchestrator core corefilter json)
add_dependencies(hipe_server core http orchestrator json)
