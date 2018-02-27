include("${CMAKE_SOURCE_DIR}/cmake/common_functions_and_macros.cmake")
include("${CMAKE_SOURCE_DIR}/cmake/boost_config.cmake")

#orchestrator LIBRARY
add_hipe_library(orchestrator STATIC)
target_compile_definitions(orchestrator PRIVATE "-DUSE_BOOST_REGEX" "-DORCHESTRATOR_BUILD")

target_link_libraries(orchestrator ${Boost_LIBRARIES})
target_link_libraries(orchestrator ${CMAKE_THREAD_LIBS_INIT})
target_link_libraries(orchestrator core)
#target_link_libraries(orchestrator data)
target_link_libraries(orchestrator corefilter)
target_link_libraries(orchestrator json)


if(MSYS) #TODO: Is MSYS true when MSVC is true?
  message(STATUS "Test if it works with windows")
    target_link_libraries(binary ws2_32 wsock32)
  target_link_libraries(binary ws2_32 wsock32)

endif()

install_header_to_target(orchestrator)

install (TARGETS orchestrator CONFIGURATIONS Debug DESTINATION lib/Debug)
install (TARGETS orchestrator CONFIGURATIONS Release DESTINATION lib/Release)