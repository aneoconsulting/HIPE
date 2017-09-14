cmake_minimum_required (VERSION 3.7.1)

include("${CMAKE_SOURCE_DIR}/cmake/OwnerConfigPath.cmake")
include("${CMAKE_SOURCE_DIR}/cmake/live_config.cmake")
include("${CMAKE_SOURCE_DIR}/cmake/ffmpeg_config.cmake")
include("${CMAKE_SOURCE_DIR}/cmake/x264_config.cmake")
include("${CMAKE_SOURCE_DIR}/cmake/boost_config.cmake")
set(Hype_INCLUDEDIR "../../header/")

set(_src_unit_test_path ${CMAKE_CURRENT_SOURCE_DIR})
file(
    GLOB_RECURSE _source_list 
    LIST_DIRECTORIES false
    "${_src_unit_test_path}/*.c*"
)

set(_src_unit_test_inc_path ${CMAKE_SOURCE_DIR}/tests/source/)
file(
    GLOB_RECURSE _header_list 
    LIST_DIRECTORIES false
    "${_src_unit_test_inc_path}/*.h*"
)

##Include OpenCV 
find_package( OpenCV REQUIRED )
include_directories(unit_test ${OpenCV_INCLUDE_DIRS} )


##unit_test LIBRARY
add_executable(unit_test ${_source_list} ${_header_list})

target_compile_definitions(unit_test PUBLIC "-DUSE_BOOST_REGEX")
include_directories(unit_test ${Hype_INCLUDEDIR})
target_link_libraries(unit_test ${Boost_LIBRARIES})
target_link_libraries(unit_test ${CMAKE_THREAD_LIBS_INIT})
target_link_libraries( unit_test ${OpenCV_LIBS} )


foreach(_source IN ITEMS ${_source_list})
	
    get_filename_component(_source_path "${_source}" PATH)
    file(RELATIVE_PATH _source_path_rel "${_src_unit_test_path}" "${_source_path}")
    string(REPLACE "/" "\\" _group_path "${_source_path_rel}")
	message(STATUS "item source = ${_group_path}") 
    source_group("source\\${_group_path}" FILES "${_source}")
endforeach()

foreach(_source IN ITEMS ${_header_list})
    get_filename_component(_source_path "${_source}" PATH)
    file(RELATIVE_PATH _source_path_rel "${_src_unit_test_inc_path}" "${_source_path}")
    string(REPLACE "/" "\\" _group_path "${_source_path_rel}")
    source_group("header\\${_group_path}" FILES "${_source}")
endforeach()

include_directories(unit_test ${_src_unit_test_inc_path})

if(MSYS) #TODO: Is MSYS true when MSVC is true?
	message(STATUS "Test if it works with windows") 
    target_link_libraries(unit_test ws2_32 wsock32)
	target_link_libraries(unit_test ws2_32 wsock32)
   
endif()




target_link_libraries(unit_test core)
target_link_libraries(unit_test filter)
target_link_libraries(unit_test http)
target_link_libraries(unit_test json)
target_link_libraries(unit_test orchestrator)
target_link_libraries(unit_test streaming)
target_link_libraries(unit_test ${FFmepg_LIBRARIES})
target_link_libraries(unit_test ${x264_LIBRARIES})
target_link_libraries(unit_test ${Live_LIBRARIES})
target_link_libraries(unit_test ${OpenCV_LIBS})
target_link_libraries(unit_test ${Boost_LIBRARIES})
target_link_libraries(unit_test ${CMAKE_THREAD_LIBS_INIT})
if (USE_GPERFTOOLS)
  target_link_libraries(unit_test ${GPERFTOOLS_DIR}/lib/libtcmalloc.a /usr/lib/aarch64-linux-gnu/libunwind.a /usr/lib/aarch64-linux-gnu/liblzma.a)
endif()


enable_testing()
#add_subdirectory(tests)

install(TARGETS unit_test DESTINATION ../../target/)
#install(TARGETS unit_test DESTINATION /usr/local/bin/)
