include("${CMAKE_SOURCE_DIR}/cmake/common_functions_and_macros.cmake")
include("${CMAKE_SOURCE_DIR}/cmake/boost_config.cmake")
include("${CMAKE_SOURCE_DIR}/cmake/livemedia_config.cmake")
include("${CMAKE_SOURCE_DIR}/cmake/ffmpeg_config.cmake")
include("${CMAKE_SOURCE_DIR}/cmake/opencv_config.cmake")

add_hipe_library(streaming_rtsp)

target_compile_definitions(streaming_rtsp PRIVATE "-DUSE_BOOST_REGEX" "-DSTREAMING_RTSP_BUILD")

target_link_libraries(streaming_rtsp ${Boost_LIBRARIES})
target_link_libraries(streaming_rtsp ${CMAKE_THREAD_LIBS_INIT})
target_link_libraries(streaming_rtsp ${Live_LIBRARIES})
target_link_libraries(streaming_rtsp ${OpenCV_LIBS})
target_link_libraries(streaming_rtsp ${FFmpeg_LIBRARIES})
target_link_libraries(streaming_rtsp core)

prepend_target_include_directories_if_necessary(streaming_rtsp "${OpenCV_INCLUDE_DIRS}")
prepend_target_include_directories_if_necessary(streaming_rtsp "${FFmpeg_INCLUDEDIR}")
prepend_target_include_directories_if_necessary(streaming_rtsp "${Boost_INCLUDE_DIRS}")
prepend_target_include_directories_if_necessary(streaming_rtsp "${Live_INCLUDEDIR}")
# FIXME
# Why was this here?
# prepend_target_include_directories_if_necessary(hipe_server "${Live_INCLUDEDIR}")
prepend_target_include_directories_if_necessary(streaming_rtsp "${_src_streaming_rtsp_inc_path}")

# FIXME
# See notes in source/core/CMakeLists.txt

if(WIN32) #TODO: Is MSYS true when MSVC is true?
  message(STATUS "Test if it works with windows")
  target_link_libraries(streaming_rtsp ws2_32 wsock32)
  target_link_libraries(streaming_rtsp ws2_32 shlwapi)
endif(WIN32)

display_target_properties(streaming_rtsp)
