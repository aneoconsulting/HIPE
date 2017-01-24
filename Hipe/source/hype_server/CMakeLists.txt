cmake_minimum_required (VERSION 3.7.1)


include("${CMAKE_SOURCE_DIR}/boost_config.cmake")
set(Hype_INCLUDEDIR "../../header/")

set(_src_hype_server_path ${CMAKE_CURRENT_SOURCE_DIR})
file(
    GLOB_RECURSE _source_list 
    LIST_DIRECTORIES false
    "${_src_hype_server_path}/*.c*"
)

set(_src_hype_server_inc_path ${CMAKE_SOURCE_DIR}/header/hype_server)
file(
    GLOB_RECURSE _header_list 
    LIST_DIRECTORIES false
    "${_src_hype_server_inc_path}/*.h*"
)



##hype_server LIBRARY

add_executable(hype_server ${_source_list} ${_header_list})

target_compile_definitions(hype_server PUBLIC "-DUSE_BOOST_REGEX")
include_directories(hype_server ${Hype_INCLUDEDIR})
target_link_libraries(hype_server ${Boost_LIBRARIES})
target_link_libraries(hype_server ${CMAKE_THREAD_LIBS_INIT})



foreach(_source IN ITEMS ${_source_list})
	
    get_filename_component(_source_path "${_source}" PATH)
    file(RELATIVE_PATH _source_path_rel "${_src_hype_server_path}" "${_source_path}")
    string(REPLACE "/" "\\" _group_path "${_source_path_rel}")
	message(STATUS "item source = ${_group_path}") 
    source_group("source\\${_group_path}" FILES "${_source}")
endforeach()

foreach(_source IN ITEMS ${_header_list})
    get_filename_component(_source_path "${_source}" PATH)
    file(RELATIVE_PATH _source_path_rel "${_src_hype_server_inc_path}" "${_source_path}")
    string(REPLACE "/" "\\" _group_path "${_source_path_rel}")
    source_group("header\\${_group_path}" FILES "${_source}")
endforeach()

include_directories(hype_server ${_src_hype_server_inc_path})

if(MSYS) #TODO: Is MSYS true when MSVC is true?
	message(STATUS "Test if it works with windows") 
    target_link_libraries(hype_server ws2_32 wsock32)
	target_link_libraries(hype_server ws2_32 wsock32)
   
endif()




target_link_libraries(hype_server core)
target_link_libraries(hype_server filter)
target_link_libraries(hype_server http)


enable_testing()
#add_subdirectory(tests)

install(TARGETS hype_server DESTINATION ../../target/)
