include("${CMAKE_SOURCE_DIR}/cmake/common_functions_and_macros.cmake")
include("${CMAKE_SOURCE_DIR}/cmake/boost_config.cmake")
include("${CMAKE_SOURCE_DIR}/cmake/opencv_config.cmake")
if(USE_DLIB)
  include("${CMAKE_SOURCE_DIR}/cmake/dlib_config.cmake")
endif(USE_DLIB)
#data LIBRARY

add_hipe_library(data SHARED)

target_compile_definitions(data PRIVATE "-DUSE_BOOST_REGEX" "-DDATA_BUILD")

target_link_libraries(data ${OpenCV_LIBS})
target_link_libraries(data ${Boost_LIBRARIES})
target_link_libraries(data ${CMAKE_THREAD_LIBS_INIT})
# if (UNIX)
#   target_link_libraries(data '-Wl,--whole-archive core streaming -Wl,--no-whole-archive')
# endif(UNIX)
target_link_libraries(data core)


if(MSYS) #TODO: Is MSYS true when MSVC is true?
  message(STATUS "Test if it works with windows")
  target_link_libraries(binary ws2_32 wsock32)
  target_link_libraries(binary ws2_32 wsock32)
endif(MSYS)

display_target_properties(data)
