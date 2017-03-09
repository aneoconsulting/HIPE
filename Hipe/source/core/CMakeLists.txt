cmake_minimum_required (VERSION 3.7.1)

include("${CMAKE_SOURCE_DIR}/OwnerConfigPath.cmake")
include("${CMAKE_SOURCE_DIR}/boost_config.cmake")
set(Hype_INCLUDEDIR "../../header/")

set(_src_core_path ${CMAKE_CURRENT_SOURCE_DIR})
message(STATUS "Get source path = ${_src_core_path}" ) 
file(
    GLOB_RECURSE _source_list 
    LIST_DIRECTORIES false
    "${_src_core_path}/*.c*"
)

set(_src_core_inc_path ${CMAKE_SOURCE_DIR}/header/core)
file(
    GLOB_RECURSE _header_list 
    LIST_DIRECTORIES false
    "${_src_core_inc_path}/*.h*"
)



##core LIBRARY

add_library(core STATIC ${_source_list} ${_header_list})

target_compile_definitions(core PUBLIC "-DUSE_BOOST_REGEX")
include_directories(core ${Hype_INCLUDEDIR})
target_link_libraries(core ${Boost_LIBRARIES})
target_link_libraries(core ${CMAKE_THREAD_LIBS_INIT})



foreach(_source IN ITEMS ${_source_list})
	message(STATUS "item source = ${_source}") 
    get_filename_component(_source_path "${_source}" PATH)
    file(RELATIVE_PATH _source_path_rel "${_src_core_path}" "${_source_path}")
    string(REPLACE "/" "\\" _group_path "${_source_path_rel}")
	
    source_group("source\\${_group_path}" FILES "${_source}")
endforeach()

foreach(_source IN ITEMS ${_header_list})
    get_filename_component(_source_path "${_source}" PATH)
    file(RELATIVE_PATH _source_path_rel "${_src_core_inc_path}" "${_source_path}")
    string(REPLACE "/" "\\" _group_path "${_source_path_rel}")
    source_group("header\\${_group_path}" FILES "${_source}")
endforeach()

include_directories(core ${_src_core_inc_path})


if(MSYS) #TODO: Is MSYS true when MSVC is true?
	message(STATUS "Test if it works with windows") 
    target_link_libraries(binary ws2_32 wsock32)
	target_link_libraries(binary ws2_32 wsock32)
   
endif()


install(TARGETS core DESTINATION ../../target/)
#install(TARGETS core DESTINATION /usr/local/lib/)
