cmake_minimum_required (VERSION 3.7.1)


include("${CMAKE_SOURCE_DIR}/cmake/boost_config.cmake")
include("${CMAKE_SOURCE_DIR}/cmake/sourcey_config.cmake")

set(Hipe_INCLUDEDIR "../../header/")

set(_src_webrtcrecorder_path ${CMAKE_CURRENT_SOURCE_DIR})
message(STATUS "Get source path = ${_src_webrtcrecorder_path}" ) 
file(
    GLOB_RECURSE _source_list 
    LIST_DIRECTORIES false
    "${_src_webrtcrecorder_path}/*.c*"
)

set(_src_webrtcrecorder_inc_path ${CMAKE_SOURCE_DIR}/header/webrtcrecorder)
file(
    GLOB_RECURSE _header_list 
    LIST_DIRECTORIES false
    "${_src_webrtcrecorder_inc_path}/*.h*"
)



##webrtcrecorder LIBRARY

add_library(webrtcrecorder STATIC ${_source_list} ${_header_list})

target_compile_definitions(webrtcrecorder PUBLIC "-DUSE_BOOST_REGEX")
include_directories(webrtcrecorder ${Hipe_INCLUDEDIR})
include_directories(webrtcrecorder ${sourcey_INCLUDEDIR})
target_link_libraries(webrtcrecorder ${CMAKE_THREAD_LIBS_INIT})
target_link_libraries(webrtcrecorder core)



foreach(_source IN ITEMS ${_source_list})
	message(STATUS "item source = ${_source}") 
    get_filename_component(_source_path "${_source}" PATH)
    file(RELATIVE_PATH _source_path_rel "${_src_webrtcrecorder_path}" "${_source_path}")
    string(REPLACE "/" "\\" _group_path "${_source_path_rel}")
	
    source_group("source\\${_group_path}" FILES "${_source}")
endforeach()

foreach(_source IN ITEMS ${_header_list})
    get_filename_component(_source_path "${_source}" PATH)
    file(RELATIVE_PATH _source_path_rel "${_src_webrtcrecorder_inc_path}" "${_source_path}")
    string(REPLACE "/" "\\" _group_path "${_source_path_rel}")
    source_group("header\\${_group_path}" FILES "${_source}")
endforeach()

include_directories(webrtcrecorder ${_src_webrtcrecorder_inc_path})


if(MSYS) #TODO: Is MSYS true when MSVC is true?
	message(STATUS "Test if it works with windows") 
    target_link_libraries(binary ws2_32 wsock32)
	target_link_libraries(binary ws2_32 wsock32)
   
endif()

install(TARGETS webrtcrecorder DESTINATION ../../target/)
#install(TARGETS webrtcrecorder DESTINATION /usr/local/lib/)
