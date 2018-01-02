include("${CMAKE_SOURCE_DIR}/cmake/common_functions_and_macros.cmake")
include("${CMAKE_SOURCE_DIR}/cmake/boost_config.cmake")

#http LIBRARY

add_hipe_library(http)

target_compile_definitions(http PRIVATE "-DUSE_BOOST_REGEX" "-DHTTP_BUILD")
target_link_libraries(http ${Boost_LIBRARIES})
target_link_libraries(http ${CMAKE_THREAD_LIBS_INIT})
target_link_libraries(http core)
target_link_libraries(http data)
target_link_libraries(http filter)
target_link_libraries(http json)
target_link_libraries(http orchestrator)

prepend_target_include_directories_if_necessary(http "${Boost_INCLUDE_DIRS}")

if(MSYS) #TODO: Is MSYS true when MSVC is true?
  message(STATUS "Test if it works with windows")
    target_link_libraries(binary ws2_32 wsock32)
  target_link_libraries(binary ws2_32 wsock32)
endif()
