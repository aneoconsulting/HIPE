cmake_minimum_required (VERSION 3.7.1)


#include("${CMAKE_SOURCE_DIR}/boost_config.cmake")
include("${CMAKE_SOURCE_DIR}/ffmpeg_config.cmake")


find_package(OpenCV REQUIRED)

set(Hype_INCLUDEDIR "../../header/")

set(_src_ffmpegRecorder_path ${CMAKE_CURRENT_SOURCE_DIR})
message(STATUS "Get source path = ${_src_ffmpegRecorder_path}" ) 
file(
    GLOB_RECURSE _source_list 
    LIST_DIRECTORIES false
    "${_src_ffmpegRecorder_path}/*.c*"
)

set(_src_ffmpegRecorder_inc_path ${CMAKE_SOURCE_DIR}/source/ffmpegRecorder)
file(
    GLOB_RECURSE _header_list 
    LIST_DIRECTORIES false
    "${_src_ffmpegRecorder_inc_path}/*.h*"
)


##ffmpegRecorder LIBRARY

#add_library(ffmpegRecorder STATIC ${_source_list} ${_header_list})
add_executable(ffmpegRecorder ${_source_list} ${_header_list})

target_compile_definitions(ffmpegRecorder PUBLIC "-DUSE_BOOST_REGEX")


foreach(_source IN ITEMS ${_source_list})
	message(STATUS "item source = ${_source}") 
    get_filename_component(_source_path "${_source}" PATH)
    file(RELATIVE_PATH _source_path_rel "${_src_ffmpegRecorder_path}" "${_source_path}")
    string(REPLACE "/" "\\" _group_path "${_source_path_rel}")
	
    source_group("source\\${_group_path}" FILES "${_source}")
endforeach()

foreach(_source IN ITEMS ${_header_list})
    get_filename_component(_source_path "${_source}" PATH)
    file(RELATIVE_PATH _source_path_rel "${_src_ffmpegRecorder_inc_path}" "${_source_path}")
    string(REPLACE "/" "\\" _group_path "${_source_path_rel}")
    source_group("header\\${_group_path}" FILES "${_source}")
endforeach()

include_directories(ffmpegRecorder ${Hype_INCLUDEDIR})
include_directories(ffmpegRecorder ${FFmpeg_INCLUDEDIR})
include_directories(ffmpegRecorder ${OpenCV_INCLUDE_DIRS} )
include_directories(ffmpegRecorder ${_src_ffmpegRecorder_inc_path})

target_link_libraries(ffmpegRecorder ${CMAKE_THREAD_LIBS_INIT})
target_link_libraries(ffmpegRecorder ${FFmpeg_LIBRARIES})
target_link_libraries( ffmpegRecorder ${OpenCV_LIBS} )

target_link_libraries(ffmpegRecorder core)


if(WIN32) #TODO: Is MSYS true when MSVC is true?
	message(STATUS "Test if it works with windows") 
    target_link_libraries(ffmpegRecorder ws2_32 wsock32 ws2_32 wsock32 Winmm Secur32 Msdmo Dmoguids	wmcodecdspuuid strmiids	Psapi Userenv)
	
   
endif()

install(TARGETS ffmpegRecorder DESTINATION ../../target/)
#install(TARGETS ffmpegRecorder DESTINATION /usr/local/lib/)
