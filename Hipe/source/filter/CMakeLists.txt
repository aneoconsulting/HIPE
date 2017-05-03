cmake_minimum_required (VERSION 3.7.1)



# Helper function to add preprocesor definition of FILE_BASENAME
# to pass the filename without directory path for debugging use.
#
# Example:
#
#   define_file_basename_for_sources(my_target)
#
# Will add -DFILE_BASENAME="filename" for each source file depended on
# by my_target, where filename is the name of the file.
#
function(define_file_basename_for_sources targetname)
    get_target_property(source_files "${targetname}" SOURCES)
    foreach(sourcefile ${source_files})
        # Get source file's current list of compile definitions.
        get_property(defs SOURCE "${sourcefile}"
            PROPERTY COMPILE_DEFINITIONS)
        # Add the FILE_BASENAME=filename compile definition to the list.
        get_filename_component(basename "${sourcefile}" NAME)
		string(REGEX MATCH "^(.*)\\.[^.]*$" dummy ${basename})
		set(MYFILE_WITHOUT_EXT ${CMAKE_MATCH_1})
        list(APPEND defs "FILE_BASENAME=${MYFILE_WITHOUT_EXT}")
        # Set the updated compile definitions on the source file.
        set_property(
            SOURCE "${sourcefile}"
            PROPERTY COMPILE_DEFINITIONS ${defs})
    endforeach()
endfunction()


include("${CMAKE_SOURCE_DIR}/OwnerConfigPath.cmake")
include("${CMAKE_SOURCE_DIR}/boost_config.cmake")
include("${CMAKE_SOURCE_DIR}/live_config.cmake")
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
  SET(script_compy "${CMAKE_SOURCE_DIR}/scripts/compy.bat")
  FILE(TO_NATIVE_PATH "${script_compy}" os_path)
  SET(script_compy ${os_path})
endif (WIN32)

FILE(TO_NATIVE_PATH "${script_path}" os_path)
SET(script_path ${os_path})

FILE(TO_NATIVE_PATH "${header_ref}" os_ref_header_path)
FILE(TO_NATIVE_PATH "${header_ref}_test" os_ref_header_path_test)


##Include OpenCV 
#SET(OpenCV_DIR "C:\\workspace\\ANEO\\Project\\HipeExternal\\win64\\opencv\\install_64bits")
message(STATUS "Variable path for OpenCV [${OpenCV_DIR}]")

find_package( OpenCV REQUIRED )

include_directories(filter ${Hype_INCLUDEDIR})
include_directories(filter ${OpenCV_INCLUDE_DIRS} )
include_directories(filter ${Live_INCLUDEDIR} )

##filter LIBRARY

add_library(filter SHARED
  ${_source_list} ${_header_list} ${header_ref})

define_file_basename_for_sources(filter)
 

if (WIN32)

	add_custom_command(TARGET filter
		PRE_BUILD
		COMMAND cmd /c echo \#pragma once > "${os_ref_header_path}.cmp"
		COMMAND cmd /c echo ^// Algorithm header to reference  >> \"${os_ref_header_path}.cmp\"
		COMMAND cmd /c "${script_path}" "filter/Algos" >> \"${os_ref_header_path}.cmp\"
		COMMAND cmd /c "${script_compy}" \"${os_ref_header_path}.cmp\" \"${os_ref_header_path}\"
		COMMENT "Generate reference file"
		WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/header/filter/Algos
	)
	#ADD_CUSTOM_TARGET(reference
	#	COMMAND cmd /c echo \#pragma once > "${os_ref_header_path}"
	#	COMMAND cmd /c echo ^// Algorithm header to reference  >> "${os_ref_header_path}"
	#	COMMAND cmd /c "${script_path}" "filter/Algos" >> "${os_ref_header_path}"
	#	COMMENT "Generate reference file"
	#	WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/header/filter/Algos
	#)


endif (WIN32)

if (UNIX)
  add_custom_target(reference
	COMMAND echo "Start generation of references"
	WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/header/filter/Algos)
  
  add_custom_command(TARGET reference
	POST_BUILD
	COMMAND echo "\\\#pragma once" > "${os_ref_header_path}.cmp"
	COMMAND echo "// Algorithm header to reference"  >> "${os_ref_header_path}.cmp"
    COMMAND bash "${script_path}" "filter/Algos" >> "${os_ref_header_path}.cmp"
	COMMAND rsync -cr "${os_ref_header_path}.cmp" "${os_ref_header_path}"
	COMMENT "Generate reference file"
	WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/header/filter/Algos
	
	)
  add_dependencies(filter reference)

endif (UNIX) 

if (UNIX)
	target_compile_options(filter PRIVATE -fPIC)
endif()

set_property(TARGET filter PROPERTY POSITION_INDEPENDENT_CODE ON)

target_compile_definitions(filter PRIVATE "-DUSE_BOOST_REGEX" "-DBUILDING_DLL")



target_link_libraries(filter ${Boost_LIBRARIES})
target_link_libraries(filter ${CMAKE_THREAD_LIBS_INIT})
target_link_libraries( filter ${OpenCV_LIBS} )
target_link_libraries( filter ${Lives_LIBS} )

#add_dependencies(filter reference)


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
if (UNIX)
	SET (MYLIBS -Wl,--whole-archive streaming core -Wl,--no-whole-archive)
endif()

if(WIN32)
	SET (MYLIBS streaming core)
endif()

target_link_libraries(filter ${MYLIBS} )

if(WIN32) #TODO: Is MSYS true when MSVC is true?
	message(STATUS "Test if it works with windows") 
    target_link_libraries(filter ws2_32 wsock32)
   
endif()

#TODO ^ is there something like that to do on Linux ?


install(TARGETS filter DESTINATION ../../target/)
#install(TARGETS filter DESTINATION /usr/local/lib/) 
