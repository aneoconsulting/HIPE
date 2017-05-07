cmake_minimum_required (VERSION 3.7.1)


include("${CMAKE_SOURCE_DIR}/boost_config.cmake")
include("${CMAKE_SOURCE_DIR}/live_config.cmake")
include("${CMAKE_SOURCE_DIR}/ffmpeg_config.cmake")

set(Hype_INCLUDEDIR "../../header/")

set(_src_streaming_rtsp_path ${CMAKE_CURRENT_SOURCE_DIR})
message(STATUS "Get source path = ${_src_streaming_rtsp_path}" ) 
file(
    GLOB_RECURSE _source_list 
    LIST_DIRECTORIES false
    "${_src_streaming_rtsp_path}/*.c*"
)

set(_src_streaming_rtsp_inc_path ${CMAKE_SOURCE_DIR}/header/streaming_rtsp)
file(
    GLOB_RECURSE _header_list 
    LIST_DIRECTORIES false
    "${_src_streaming_rtsp_inc_path}/*.h*"
)


##Include OpenCV 
find_package( OpenCV REQUIRED )

##streaming_rtsp LIBRARY

add_library(streaming_rtsp STATIC ${_source_list} ${_header_list})

if (UNIX)
	target_compile_options(streaming_rtsp PRIVATE -fPIC)
endif()


target_compile_definitions(streaming_rtsp PRIVATE "-DUSE_BOOST_REGEX" "-DSTREAMING_RTSP_BUILD")


target_link_libraries(streaming_rtsp ${Boost_LIBRARIES})
target_link_libraries(streaming_rtsp ${CMAKE_THREAD_LIBS_INIT})
target_link_libraries(streaming_rtsp ${Live_LIBRARIES})
target_link_libraries(streaming_rtsp core)
target_link_libraries(streaming_rtsp ${OpenCV_LIBS})
target_link_libraries(streaming_rtsp ${FFmpeg_LIBRARIES})


foreach(_source IN ITEMS ${_source_list})
	message(STATUS "item source = ${_source}") 
    get_filename_component(_source_path "${_source}" PATH)
    file(RELATIVE_PATH _source_path_rel "${_src_streaming_rtsp_path}" "${_source_path}")
    string(REPLACE "/" "\\" _group_path "${_source_path_rel}")
	
    source_group("source\\${_group_path}" FILES "${_source}")
endforeach()

foreach(_source IN ITEMS ${_header_list})
    get_filename_component(_source_path "${_source}" PATH)
    file(RELATIVE_PATH _source_path_rel "${_src_streaming_rtsp_inc_path}" "${_source_path}")
    string(REPLACE "/" "\\" _group_path "${_source_path_rel}")
    source_group("header\\${_group_path}" FILES "${_source}")
endforeach()

include_directories(streaming_rtsp ${Hype_INCLUDEDIR})
include_directories(streaming_rtsp ${OpenCV_INCLUDE_DIRS} )
include_directories(streaming_rtsp ${FFmpeg_INCLUDEDIR} )
include_directories(hype_server ${Live_INCLUDEDIR})
include_directories(streaming_rtsp ${_src_streaming_rtsp_inc_path})


if(WIN32) #TODO: Is MSYS true when MSVC is true?
	message(STATUS "Test if it works with windows") 
    target_link_libraries(streaming_rtsp ws2_32 wsock32)
	target_link_libraries(streaming_rtsp ws2_32 shlwapi )
   
endif()

install(TARGETS streaming_rtsp DESTINATION ../../target/)
#install(TARGETS streaming_rtsp DESTINATION /usr/local/lib/)
