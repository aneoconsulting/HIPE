cmake_minimum_required (VERSION 3.7.1)


include("${CMAKE_SOURCE_DIR}/boost_config.cmake")
include("${CMAKE_SOURCE_DIR}/sourcey_config.cmake")
include("${CMAKE_SOURCE_DIR}/webrtc_config.cmake")
include("${CMAKE_SOURCE_DIR}/ffmpeg_config.cmake")
include("${CMAKE_SOURCE_DIR}/libuv_config.cmake")


find_package(OpenCV REQUIRED)

set(Hype_INCLUDEDIR "../../header/")

set(_src_webrtcstreamer_path ${CMAKE_CURRENT_SOURCE_DIR})
message(STATUS "Get source path = ${_src_webrtcstreamer_path}" ) 
file(
    GLOB_RECURSE _source_list 
    LIST_DIRECTORIES false
    "${_src_webrtcstreamer_path}/*.c*"
)

set(_src_webrtcstreamer_inc_path ${CMAKE_SOURCE_DIR}/source/webrtcstreamer)
file(
    GLOB_RECURSE _header_list 
    LIST_DIRECTORIES false
    "${_src_webrtcstreamer_inc_path}/*.h*"
)


##webrtcstreamer LIBRARY

#add_library(webrtcstreamer STATIC ${_source_list} ${_header_list})
add_executable(webrtcstreamer ${_source_list} ${_header_list})

target_compile_definitions(webrtcstreamer PUBLIC "-DUSE_BOOST_REGEX" "-DWEBRTC_WIN")


foreach(_source IN ITEMS ${_source_list})
	message(STATUS "item source = ${_source}") 
    get_filename_component(_source_path "${_source}" PATH)
    file(RELATIVE_PATH _source_path_rel "${_src_webrtcstreamer_path}" "${_source_path}")
    string(REPLACE "/" "\\" _group_path "${_source_path_rel}")
	
    source_group("source\\${_group_path}" FILES "${_source}")
endforeach()

foreach(_source IN ITEMS ${_header_list})
    get_filename_component(_source_path "${_source}" PATH)
    file(RELATIVE_PATH _source_path_rel "${_src_webrtcstreamer_inc_path}" "${_source_path}")
    string(REPLACE "/" "\\" _group_path "${_source_path_rel}")
    source_group("header\\${_group_path}" FILES "${_source}")
endforeach()

include_directories(webrtcstreamer ${Hype_INCLUDEDIR})
include_directories(webrtcstreamer ${sourcey_INCLUDEDIR})
include_directories(webrtcstreamer ${webrtc_INCLUDEDIR})
include_directories(webrtcstreamer ${FFmpeg_INCLUDEDIR})
include_directories(webrtcstreamer ${uv_INCLUDEDIR})
include_directories(webrtcstreamer ${OpenCV_INCLUDE_DIRS} )
include_directories(webrtcstreamer ${_src_webrtcstreamer_inc_path})

target_link_libraries(webrtcstreamer ${CMAKE_THREAD_LIBS_INIT})
target_link_libraries(webrtcstreamer ${FFmpeg_LIBRARIES})
target_link_libraries(webrtcstreamer ${webrtc_LIBRARIES})
target_link_libraries(webrtcstreamer ${sourcey_LIBRARIES})
target_link_libraries(webrtcstreamer ${PROTOBUF_LIBRARY})
target_link_libraries(webrtcstreamer ${uv_LIBRARIES})
target_link_libraries( webrtcstreamer ${OpenCV_LIBS} )

target_link_libraries(webrtcstreamer core)


if(WIN32) #TODO: Is MSYS true when MSVC is true?
	message(STATUS "Test if it works with windows") 
    target_link_libraries(webrtcstreamer ws2_32 wsock32 ws2_32 wsock32 Winmm Secur32 Msdmo Dmoguids	wmcodecdspuuid strmiids	Psapi Userenv)
	
   
endif()

install(TARGETS webrtcstreamer DESTINATION ../../target/)
#install(TARGETS webrtcstreamer DESTINATION /usr/local/lib/)
