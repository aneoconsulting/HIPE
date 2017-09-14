cmake_minimum_required (VERSION 3.7.1)


include("${CMAKE_SOURCE_DIR}/cmake/boost_config.cmake")
set(Hype_INCLUDEDIR "../../header/")

set(_src_json_path ${CMAKE_CURRENT_SOURCE_DIR})
message(STATUS "Get source path = ${_src_json_path}" ) 
file(
    GLOB_RECURSE _source_list 
    LIST_DIRECTORIES false
    "${_src_json_path}/*.c*"
)

set(_src_json_inc_path ${CMAKE_SOURCE_DIR}/header/json)
file(
    GLOB_RECURSE _header_list 
    LIST_DIRECTORIES false
    "${_src_json_inc_path}/*.h*"
)



##json LIBRARY

add_library(json STATIC ${_source_list} ${_header_list})

target_compile_definitions(json PRIVATE "-DUSE_BOOST_REGEX" "-DJSON_BUILD")
include_directories(json ${Hype_INCLUDEDIR})
target_link_libraries(json ${Boost_LIBRARIES})
target_link_libraries(json ${CMAKE_THREAD_LIBS_INIT})
target_link_libraries(json filter)
target_link_libraries(json core)



foreach(_source IN ITEMS ${_source_list})
	message(STATUS "item source = ${_source}") 
    get_filename_component(_source_path "${_source}" PATH)
    file(RELATIVE_PATH _source_path_rel "${_src_json_path}" "${_source_path}")
    string(REPLACE "/" "\\" _group_path "${_source_path_rel}")
	
    source_group("source\\${_group_path}" FILES "${_source}")
endforeach()

foreach(_source IN ITEMS ${_header_list})
    get_filename_component(_source_path "${_source}" PATH)
    file(RELATIVE_PATH _source_path_rel "${_src_json_inc_path}" "${_source_path}")
    string(REPLACE "/" "\\" _group_path "${_source_path_rel}")
    source_group("header\\${_group_path}" FILES "${_source}")
endforeach()

include_directories(json ${_src_json_inc_path})


if(MSYS) #TODO: Is MSYS true when MSVC is true?
	message(STATUS "Test if it works with windows") 
    target_link_libraries(binary ws2_32 wsock32)
	target_link_libraries(binary ws2_32 wsock32)
   
endif()

install(TARGETS json DESTINATION ../../target/)
#install(TARGETS json DESTINATION /usr/local/lib/)
