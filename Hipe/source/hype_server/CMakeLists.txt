cmake_minimum_required (VERSION 3.7.1)

include("${CMAKE_SOURCE_DIR}/OwnerConfigPath.cmake")
include("${CMAKE_SOURCE_DIR}/boost_config.cmake")
include("${CMAKE_SOURCE_DIR}/live_config.cmake")
include("${CMAKE_SOURCE_DIR}/ffmpeg_config.cmake")
include("${CMAKE_SOURCE_DIR}/x264_config.cmake")

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

##Include OpenCV 
find_package( OpenCV REQUIRED )

if (USE_DLIB)
  include("${CMAKE_SOURCE_DIR}/dlib_config.cmake")
  set(dlib_DIR "${DLIB_DIR}/lib/cmake/dlib/")

  find_package( dlib REQUIRED )
endif()



##hype_server LIBRARY
add_executable(hype_server ${_source_list} ${_header_list})

target_compile_definitions(hype_server PRIVATE "-DUSE_BOOST_REGEX")



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



if(MSYS) #TODO: Is MSYS true when MSVC is true?
	message(STATUS "Test if it works with windows") 
    target_link_libraries(hype_server ws2_32 wsock32)
	target_link_libraries(hype_server ws2_32 wsock32)
   
endif()



include_directories(hype_server ${Hype_INCLUDEDIR})
include_directories(hype_server ${Live_INCLUDEDIR})
include_directories(hype_server ${OpenCV_INCLUDE_DIRS} )
include_directories(hype_server ${_src_hype_server_inc_path})

message(STATUS "Live LIBRARIES : ${Live_LIBRARIES}")

target_link_libraries(hype_server core)

target_link_libraries(hype_server http)
target_link_libraries(hype_server json)
target_link_libraries(hype_server orchestrator)
target_link_libraries(hype_server streaming)
if (USE_DLIB)
  target_link_libraries(hype_server dlib::dlib_shared)
endif()

message(STATUS "Library ffmpeg to include ${FFmpeg_LIBRARIES}") 
message(STATUS "Library x264 to include ${x264_LIBRARIES}") 
message(STATUS "Library Live to include ${Live_LIBRARIES}") 
message(STATUS "Library OpenCV to include ${OpenCV_LIBS}") 
message(STATUS "Library Boost to include ${Boost_LIBRARIES}") 
target_link_libraries(hype_server ${FFmpeg_LIBRARIES})
target_link_libraries(hype_server ${x264_LIBRARIES})
target_link_libraries(hype_server ${Live_LIBRARIES})
target_link_libraries(hype_server ${OpenCV_LIBS})
target_link_libraries(hype_server ${Boost_LIBRARIES})
target_link_libraries(hype_server ${CMAKE_THREAD_LIBS_INIT})

if (USE_GPERFTOOLS)
  target_link_libraries(hype_server ${GPERFTOOLS_DIR}/lib/libtcmalloc.a /usr/lib/aarch64-linux-gnu/libunwind.a /usr/lib/aarch64-linux-gnu/liblzma.a)
endif()



enable_testing()
#add_subdirectory(tests)

install(TARGETS hype_server DESTINATION ../../target/)
#install(TARGETS hype_server DESTINATION /usr/local/bin/)
