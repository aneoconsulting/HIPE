cmake_minimum_required (VERSION 3.7.1)


include("${CMAKE_SOURCE_DIR}/boost_config.cmake")
set(Hype_INCLUDEDIR "../../header/")

set(_src_filter_path ${CMAKE_CURRENT_SOURCE_DIR})
file(
    GLOB_RECURSE _source_list 
    LIST_DIRECTORIES false
    "${_src_filter_path}/*.c*"
)

set(_src_filter_inc_path ${CMAKE_SOURCE_DIR}/header/filter)
file(
    GLOB_RECURSE _header_list 
    LIST_DIRECTORIES false
    "${_src_filter_inc_path}/*.h*"
)

##Generate header file of any algorithms into References.h
# Find all the input files
FILE(GLOB_RECURSE inFiles 
	LIST_DIRECTORIES false 
	RELATIVE "${_src_filter_inc_path}"
    "${_src_filter_inc_path}/*.h*")
	
set(_includes "#pragma once\n")
foreach(_source IN ITEMS ${inFiles})
    #get_filename_component(_source_path "${_source}" PATH)
    #file(RELATIVE_PATH _source_path_rel "${_src_filter_inc_path}" "${_source_path}")
	message(STATUS "file are : ${_source}")
	set(_includes "${_includes}#include <filter/${_source}>\n")
    #string(REPLACE "/" "\\" _group_path "${_source_path_rel}")
    
endforeach()

message(STATUS "references header is : ${_includes}")
set(header_ref ${CMAKE_SOURCE_DIR}/header/filter/References.h)
FILE(WRITE ${CMAKE_SOURCE_DIR}/header/filter/References.h "#pragma once\n #error \"TO GENERATE IS THE BUILD\"")
message(STATUS "WRITE header into : ${header_ref}")

if (UNIX)
  SET(script_path "${CMAKE_SOURCE_DIR}/scripts/headerList.sh")
endif (UNIX)
if (WIN32)
  SET(script_path "${CMAKE_SOURCE_DIR}/scripts/headerList.bat")
endif (WIN32)

FILE(TO_NATIVE_PATH "${script_path}" os_path)
SET(script_path ${os_path})

FILE(TO_NATIVE_PATH "${header_ref}" os_ref_header_path)

if (WIN32)
ADD_CUSTOM_TARGET(reference_1
	ALL
	COMMAND cmd /c echo \#pragma once > "${os_ref_header_path}"
	COMMAND cmd /c echo ^// Algorithm header to reference  >> "${os_ref_header_path}"
    COMMAND cmd /c "${script_path}" "filter/Algos" >> "${os_ref_header_path}"
	COMMENT "Generate reference file"
	WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/header/filter/Algos
)


endif (WIN32)

if (UNIX)
ADD_CUSTOM_TARGET(reference_1
	ALL
	COMMAND echo "\\\#pragma once" > "${os_ref_header_path}"
	COMMAND echo "// Algorithm header to reference"  >> "${os_ref_header_path}"
    COMMAND bash "${script_path}" "filter/Algos" >> "${os_ref_header_path}"
	COMMENT "Generate reference file"
	WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/header/filter/Algos
)

endif (UNIX)

##filter LIBRARY

add_library(filter STATIC ${_source_list} ${_header_list} ${header_ref})

target_compile_definitions(filter PUBLIC "-DUSE_BOOST_REGEX")
include_directories(filter ${Hype_INCLUDEDIR})
target_link_libraries(filter ${Boost_LIBRARIES})
target_link_libraries(filter ${CMAKE_THREAD_LIBS_INIT})

add_dependencies(filter reference_1)


foreach(_source IN ITEMS ${_source_list})
	
    get_filename_component(_source_path "${_source}" PATH)
    file(RELATIVE_PATH _source_path_rel "${_src_filter_path}" "${_source_path}")
    string(REPLACE "/" "\\" _group_path "${_source_path_rel}")
	message(STATUS "item source = ${_group_path}") 
    source_group("source\\${_group_path}" FILES "${_source}")
endforeach()

foreach(_source IN ITEMS ${_header_list})
    get_filename_component(_source_path "${_source}" PATH)
    file(RELATIVE_PATH _source_path_rel "${_src_filter_inc_path}" "${_source_path}")
    string(REPLACE "/" "\\" _group_path "${_source_path_rel}")
    source_group("header\\${_group_path}" FILES "${_source}")
endforeach()

include_directories(filter ${_src_filter_inc_path})

target_link_libraries(filter core)

if(MSYS) #TODO: Is MSYS true when MSVC is true?
	message(STATUS "Test if it works with windows") 
    target_link_libraries(filter ws2_32 wsock32)
	target_link_libraries(binary ws2_32 wsock32)
   
endif()

#TODO ^ is there something like that to do on Linux ?


install(TARGETS filter DESTINATION ../../target/)
#install(TARGETS filter DESTINATION /usr/local/lib/) 
