include("${CMAKE_SOURCE_DIR}/cmake/common_functions_and_macros.cmake")
include("${CMAKE_SOURCE_DIR}/cmake/boost_config.cmake")
include("${CMAKE_SOURCE_DIR}/cmake/livemedia_config.cmake")
include("${CMAKE_SOURCE_DIR}/cmake/ffmpeg_config.cmake")
include("${CMAKE_SOURCE_DIR}/cmake/x264_config.cmake")
include("${CMAKE_SOURCE_DIR}/cmake/opencv_config.cmake")

# TODO
# Check that this is only needed for windows.
if (WIN32)
  include("${CMAKE_SOURCE_DIR}/cmake/sourcey_config.cmake")
endif (WIN32)


#streaming LIBRARY

add_hipe_library(streaming)

target_compile_definitions(streaming PRIVATE "-DUSE_BOOST_REGEX" "-DSTREAMING_BUILD" "-DSINGLETON_EXPORT")


target_link_libraries(streaming ${Boost_LIBRARIES})
target_link_libraries(streaming ${CMAKE_THREAD_LIBS_INIT})
target_link_libraries(streaming ${OpenCV_LIBS})
target_link_libraries(streaming ${FFmpeg_LIBRARIES})
target_link_libraries(streaming ${x264_LIBRARIES})
target_link_libraries(streaming ${Live_LIBRARIES})
target_link_libraries(streaming core)
target_link_libraries(streaming streaming_rtsp)



if(WIN32) #TODO: Is MSYS true when MSVC is true?
  message(STATUS "Test if it works with windows")
  target_link_libraries(streaming ws2_32 wsock32 ws2_32 wsock32 Winmm Secur32 Msdmo Dmoguids  wmcodecdspuuid strmiids  Psapi Userenv)
endif(WIN32)

display_target_properties(streaming)
