include("${CMAKE_SOURCE_DIR}/cmake/common_functions_and_macros.cmake")
include("${CMAKE_SOURCE_DIR}/cmake/boost_config.cmake")
include("${CMAKE_SOURCE_DIR}/cmake/livemedia_config.cmake")
include("${CMAKE_SOURCE_DIR}/cmake/ffmpeg_config.cmake")
include("${CMAKE_SOURCE_DIR}/cmake/opencv_config.cmake")

add_hipe_library(streaming_rtsp STATIC)

target_compile_definitions(streaming_rtsp PRIVATE "-DUSE_BOOST_REGEX" "-DSTREAMING_RTSP_BUILD")

target_link_libraries(streaming_rtsp ${Boost_LIBRARIES})
target_link_libraries(streaming_rtsp ${CMAKE_THREAD_LIBS_INIT})
target_link_libraries(streaming_rtsp ${Live_LIBRARIES})
target_link_libraries(streaming_rtsp ${OpenCV_LIBS})
target_link_libraries(streaming_rtsp ${FFmpeg_LIBRARIES})
target_link_libraries(streaming_rtsp core)

# FIXME
# Why was this here?

# FIXME
# See notes in source/core/CMakeLists.txt

if(WIN32) #TODO: Is MSYS true when MSVC is true?
  message(STATUS "Test if it works with windows")
  target_link_libraries(streaming_rtsp ws2_32 wsock32)
  target_link_libraries(streaming_rtsp ws2_32 shlwapi)
endif(WIN32)

display_target_properties(streaming_rtsp)
