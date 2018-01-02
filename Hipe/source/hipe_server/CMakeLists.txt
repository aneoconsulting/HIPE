include("${CMAKE_SOURCE_DIR}/cmake/common_functions_and_macros.cmake")
include("${CMAKE_SOURCE_DIR}/cmake/boost_config.cmake")
include("${CMAKE_SOURCE_DIR}/cmake/livemedia_config.cmake")
include("${CMAKE_SOURCE_DIR}/cmake/ffmpeg_config.cmake")
include("${CMAKE_SOURCE_DIR}/cmake/x264_config.cmake")
include("${CMAKE_SOURCE_DIR}/cmake/opencv_config.cmake")

if(USE_DLIB)
  include("${CMAKE_SOURCE_DIR}/cmake/dlib_config.cmake")
endif(USE_DLIB)


#hipe_server LIBRARY
add_hipe_executable(hipe_server)

target_compile_definitions(hipe_server PRIVATE "-DUSE_BOOST_REGEX")


# TODO
# Is this not already included in the output of CMake? Remove?
display_pathlist("FFmpeg_LIBRARIES" "${FFmpeg_LIBRARIES}")
display_pathlist("x264_LIBRARIES" "${x264_LIBRARIES}")
display_pathlist("Live_LIBRARIES" "${Live_LIBRARIES}")
display_pathlist("OpenCV_LIBS" "${OpenCV_LIBS}")
display_pathlist("Boost_LIBRARIES" "${Boost_LIBRARIES}")
display_pathlist("Live_LIBRARIES" "${Live_LIBRARIES}")

target_link_libraries(hipe_server http)
target_link_libraries(hipe_server orchestrator)
target_link_libraries(hipe_server json)
target_link_libraries(hipe_server data)
# target_link_libraries(hipe_server streaming)
target_link_libraries(hipe_server core)
target_link_libraries(hipe_server ${FFmpeg_LIBRARIES})
target_link_libraries(hipe_server ${x264_LIBRARIES})
target_link_libraries(hipe_server ${Live_LIBRARIES})
target_link_libraries(hipe_server ${OpenCV_LIBS})
target_link_libraries(hipe_server ${Boost_LIBRARIES})
target_link_libraries(hipe_server ${CMAKE_THREAD_LIBS_INIT})

prepend_target_include_directories_if_necessary(hipe_server "${FFmpeg_INCLUDEDIR}")
prepend_target_include_directories_if_necessary(hipe_server "${Live_INCLUDEDIR}")
prepend_target_include_directories_if_necessary(hipe_server "${x264_INCLUDEDIR}")
prepend_target_include_directories_if_necessary(hipe_server "${OpenCV_INCLUDE_DIRS}")
prepend_target_include_directories_if_necessary(hipe_server "${Boost_INCLUDE_DIRS}")




if(MSYS) #TODO: Is MSYS true when MSVC is true?
  message(STATUS "Test if it works with windows")
  target_link_libraries(hipe_server ws2_32 wsock32)
  target_link_libraries(hipe_server ws2_32 wsock32)
endif(MSYS)


if(USE_DLIB)
  target_link_libraries(hipe_server ${Dlib_LIBRARIES})
#   if(WIN32)
#     target_link_libraries(hipe_server ${Dlib_LIBRARIES})
#
#   #set(BLAS_openblas_LIBRARY ${HIPE_EXTERNAL}/OpenBLAS/)
#   #find_package(BLAS REQUIRED)
#
#   elseif(UNIX)
#     target_link_libraries(hipe_server ${Dlib_LIBRARIES})
#
#   endif(WIN32)
endif(USE_DLIB)

if(USE_GPERFTOOLS)
  target_link_libraries(hipe_server ${GPERFTOOLS_DIR}/lib/libtcmalloc.a /usr/lib/aarch64-linux-gnu/libunwind.a /usr/lib/aarch64-linux-gnu/liblzma.a)
endif(USE_GPERFTOOLS)



enable_testing()
#add_subdirectory(tests)

display_target_properties(hipe_server)
