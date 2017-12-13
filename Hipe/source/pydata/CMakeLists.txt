cmake_minimum_required (VERSION 3.7.1)


include("${CMAKE_SOURCE_DIR}/cmake/boost_config.cmake")
include("${CMAKE_SOURCE_DIR}/cmake/python27_config.cmake")
set(Hipe_INCLUDEDIR "../../header/")

set(_src_pydata_path ${CMAKE_CURRENT_SOURCE_DIR})
message(STATUS "Get source path = ${_src_pydata_path}" ) 
file(
    GLOB_RECURSE _source_list 
    LIST_DIRECTORIES false
    "${_src_pydata_path}/*.c*"
)

set(_src_pydata_inc_path ${CMAKE_SOURCE_DIR}/header/pydata)
file(
    GLOB_RECURSE _header_list 
    LIST_DIRECTORIES false
    "${_src_pydata_inc_path}/*.h*"
)

message(STATUS "Variable path for OpenCV [${OpenCV_DIR}]")

find_package( OpenCV REQUIRED )

##pydata LIBRARY

add_library(pydata SHARED ${_source_list} ${_header_list})

target_compile_definitions(pydata PRIVATE "-DCV_VERSION_EPOCH=3" "-DUSE_BOOST_REGEX" "-DPYDATA_BUILD")

include_directories(pydata ${Hipe_INCLUDEDIR})
include_directories(pydata ${OpenCV_INCLUDE_DIRS} )
include_directories(pydata ${Python27_INCLUDEDIR} )

target_link_libraries(pydata ${OpenCV_LIBS})
target_link_libraries(pydata ${Boost_LIBRARIES})
target_link_libraries(pydata ${CMAKE_THREAD_LIBS_INIT})

message(STATUS "Python27 LIBRARIES for pydata : ${Python27_LIBRARIES}")
target_link_libraries(pydata ${Python27_LIBRARIES} )



foreach(_source IN ITEMS ${_source_list})
	message(STATUS "item source = ${_source}") 
    get_filename_component(_source_path "${_source}" PATH)
    file(RELATIVE_PATH _source_path_rel "${_src_pydata_path}" "${_source_path}")
    string(REPLACE "/" "\\" _group_path "${_source_path_rel}")
	
    source_group("source\\${_group_path}" FILES "${_source}")
endforeach()

foreach(_source IN ITEMS ${_header_list})
    get_filename_component(_source_path "${_source}" PATH)
    file(RELATIVE_PATH _source_path_rel "${_src_pydata_inc_path}" "${_source_path}")
    string(REPLACE "/" "\\" _group_path "${_source_path_rel}")
    source_group("header\\${_group_path}" FILES "${_source}")
endforeach()

include_directories(pydata ${_src_pydata_inc_path})

if (UNIX)
	SET (MYLIBS -Wl,--whole-archive streaming core -Wl,--no-whole-archive)
endif()

if(WIN32)
	SET (MYLIBS streaming core)
endif()

target_link_libraries(pydata ${MYLIBS} )

if(MSYS) #TODO: Is MSYS true when MSVC is true?
	message(STATUS "Test if it works with windows") 
    target_link_libraries(pydata ws2_32 wsock32)
	target_link_libraries(pydata ws2_32 wsock32)
   
endif()


add_custom_command(TARGET pydata 
                   POST_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:pydata> $<TARGET_FILE_DIR:pydata>/pydata.pyd)

install(TARGETS pydata DESTINATION ../../target/)


#install(TARGETS pydata DESTINATION /usr/local/lib/)
