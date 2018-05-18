cmake_minimum_required(VERSION 3.7.1)



set(FFmpeg_DIR "FFMPEG-ROOT-NOTFOUND" CACHE PATH "Path to the root live directory")


if(${FFmpeg_DIR} EQUAL "FFMPEG-ROOT-NOTFOUND")
  message(FATAL_ERROR "Variable FFmpeg_DIR is empty")
endif(${FFmpeg_DIR} EQUAL "FFMPEG-ROOT-NOTFOUND")

set(FFmpeg_INCLUDEDIR "${FFmpeg_DIR}/include" CACHE PATH "include path for live" FORCE)

set(FFmpeg_LIBRARDIR "${FFmpeg_DIR}/lib" CACHE PATH "include path for live" FORCE)

set(lib_path ${FFmpeg_LIBRARDIR})
set(_lib_list "")

if(WIN32)
  file(
    GLOB_RECURSE _lib_list
    LIST_DIRECTORIES false
    "${lib_path}/*.lib"
  )
else(WIN32)
#file(
#    GLOB_RECURSE _lib_list
#    LIST_DIRECTORIES false
#    "${lib_path}/*.a"
#)
  set(
    _lib_list
    ${lib_path}/libswscale.so
    ${lib_path}/libavcodec.so
    ${lib_path}/libavdevice.so
    ${lib_path}/libavfilter.so
    ${lib_path}/libavformat.so
    ${lib_path}/libavutil.so
#     libboost_wave.so
  )
endif(WIN32)

set(FFmpeg_LIBRARIES "${_lib_list}" CACHE STRING "libraries" FORCE)
prepend_include_directories_if_necessary("${FFmpeg_INCLUDEDIR}")
