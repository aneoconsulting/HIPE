cmake_minimum_required (VERSION 3.7.1)



set(FFMPEG_DIR "FFMPEG-ROOT-NOTFOUND" CACHE PATH "Path to the root live directory" )


if (${FFMPEG_DIR} EQUAL "FFMPEG-ROOT-NOTFOUND")
	message( FATAL_ERROR "Variable FFMPEG_DIR is empty")
endif()

set(FFmpeg_INCLUDEDIR "${FFMPEG_DIR}/include" CACHE PATH "include path for live" FORCE)

set(FFmpeg_LIBRARDIR "${FFMPEG_DIR}/lib" CACHE PATH "include path for live" FORCE)

set(lib_path ${FFmpeg_LIBRARDIR})
set(_lib_list "")

if (WIN32)
file(
    GLOB_RECURSE _lib_list 
    LIST_DIRECTORIES false
    "${lib_path}/*.lib"
)
else()
#file(
#    GLOB_RECURSE _lib_list 
#    LIST_DIRECTORIES false
#    "${lib_path}/*.a"
#)
set(_lib_list
		${lib_path}/libswscale.so
		${lib_path}/libavcodec.so
		${lib_path}/libavdevice.so
		${lib_path}/libavfilter.so
		${lib_path}/libavformat.so
		${lib_path}/libavutil.so
		${lib_path}/libboost_wave.so
		)
endif()

set(FFmpeg_LIBRARIES "${_lib_list}" CACHE STRING "libraries" FORCE)
