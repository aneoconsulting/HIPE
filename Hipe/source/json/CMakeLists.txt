include("${CMAKE_SOURCE_DIR}/cmake/common_functions_and_macros.cmake")
include("${CMAKE_SOURCE_DIR}/cmake/boost_config.cmake")

add_hipe_library(json)

target_compile_definitions(json PRIVATE "-DUSE_BOOST_REGEX" "-DJSON_BUILD")

target_link_libraries(json ${Boost_LIBRARIES})
target_link_libraries(json ${CMAKE_THREAD_LIBS_INIT})
target_link_libraries(json core corefilter data)

if(MSYS) #TODO: Is MSYS true when MSVC is true?
  message(STATUS "Test if it works with windows")
    target_link_libraries(binary ws2_32 wsock32)
  target_link_libraries(binary ws2_32 wsock32)
endif(MSYS)

display_target_properties(json)
