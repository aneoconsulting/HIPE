cmake_minimum_required (VERSION 3.7.1)


include("${CMAKE_SOURCE_DIR}/cmake/boost_config.cmake")
set(Hipe_INCLUDEDIR "../../header/")

if (USE_DLIB)
  include("${CMAKE_SOURCE_DIR}/cmake/dlib_config.cmake")
endif()

set(_src_data_path ${CMAKE_CURRENT_SOURCE_DIR})
message(STATUS "Get source path = ${_src_data_path}" ) 
file(
    GLOB_RECURSE _source_list 
    LIST_DIRECTORIES false
    "${_src_data_path}/*.c*"
)

set(_src_data_inc_path ${CMAKE_SOURCE_DIR}/header/data)
file(
    GLOB_RECURSE _header_list 
    LIST_DIRECTORIES false
    "${_src_data_inc_path}/*.h*"
)

message(STATUS "Variable path for OpenCV [${OpenCV_DIR}]")

find_package( OpenCV REQUIRED )

##data LIBRARY

add_library(data SHARED ${_source_list} ${_header_list})

target_compile_definitions(data PRIVATE "-DUSE_BOOST_REGEX" "-DDATA_BUILD")

include_directories(data ${Hipe_INCLUDEDIR})
include_directories(data ${OpenCV_INCLUDE_DIRS} )

if(USE_DLIB)
	include_directories(data ${Dlib_INCLUDEDIR})
endif()

target_link_libraries(data ${OpenCV_LIBS})
target_link_libraries(data ${Boost_LIBRARIES})
target_link_libraries(data ${CMAKE_THREAD_LIBS_INIT})

if (USE_DLIB)
	if (WIN32)
		target_link_libraries(data ${Dlib_LIBRARIES})
	
	elseif (UNIX)
		target_link_libraries(data ${Dlib_LIBRARIES})
	
	endif()

endif()



foreach(_source IN ITEMS ${_source_list})
	message(STATUS "item source = ${_source}") 
    get_filename_component(_source_path "${_source}" PATH)
    file(RELATIVE_PATH _source_path_rel "${_src_data_path}" "${_source_path}")
    string(REPLACE "/" "\\" _group_path "${_source_path_rel}")
	
    source_group("source\\${_group_path}" FILES "${_source}")
endforeach()

foreach(_source IN ITEMS ${_header_list})
    get_filename_component(_source_path "${_source}" PATH)
    file(RELATIVE_PATH _source_path_rel "${_src_data_inc_path}" "${_source_path}")
    string(REPLACE "/" "\\" _group_path "${_source_path_rel}")
    source_group("header\\${_group_path}" FILES "${_source}")
endforeach()

include_directories(data ${_src_data_inc_path})

if (UNIX)
	SET (MYLIBS -Wl,--whole-archive core -Wl,--no-whole-archive)
endif()

if(WIN32)
	SET (MYLIBS core)
endif()

target_link_libraries(data ${MYLIBS} )

if(MSYS) #TODO: Is MSYS true when MSVC is true?
	message(STATUS "Test if it works with windows") 
    target_link_libraries(binary ws2_32 wsock32)
	target_link_libraries(binary ws2_32 wsock32)
   
endif()

install(TARGETS data DESTINATION ../../target/)
#install(TARGETS data DESTINATION /usr/local/lib/)
