cmake_minimum_required (VERSION 3.7.1)

# include("${CMAKE_SOURCE_DIR}/cmake/OwnerConfigPath.cmake")
include("${CMAKE_SOURCE_DIR}/cmake/boost_config.cmake")
include("${CMAKE_SOURCE_DIR}/cmake/live_config.cmake")
include("${CMAKE_SOURCE_DIR}/cmake/ffmpeg_config.cmake")
include("${CMAKE_SOURCE_DIR}/cmake/x264_config.cmake")

set(Hipe_INCLUDEDIR "../../header/")

set(_src_hipe_server_path ${CMAKE_CURRENT_SOURCE_DIR})
file(
    GLOB_RECURSE _source_list
    LIST_DIRECTORIES false
    "${_src_hipe_server_path}/*.c*"
)

set(_src_hipe_server_inc_path ${CMAKE_SOURCE_DIR}/header/hipe_server)
file(
    GLOB_RECURSE _header_list
    LIST_DIRECTORIES false
    "${_src_hipe_server_inc_path}/*.h*"
)

##Include OpenCV
find_package( OpenCV REQUIRED )

if (USE_DLIB)
  include("${CMAKE_SOURCE_DIR}/cmake/dlib_config.cmake")
endif()



##hipe_server LIBRARY
add_executable(hipe_server ${_source_list} ${_header_list})
if(UNIX)
	set_target_properties(hipe_server PROPERTIES SUFFIX ".bin")
endif(UNIX)

target_compile_definitions(hipe_server PRIVATE "-DUSE_BOOST_REGEX")



foreach(_source IN ITEMS ${_source_list})

    get_filename_component(_source_path "${_source}" PATH)
    file(RELATIVE_PATH _source_path_rel "${_src_hipe_server_path}" "${_source_path}")
    string(REPLACE "/" "\\" _group_path "${_source_path_rel}")
	message(STATUS "item source = ${_group_path}")
    source_group("source\\${_group_path}" FILES "${_source}")
endforeach()

foreach(_source IN ITEMS ${_header_list})
    get_filename_component(_source_path "${_source}" PATH)
    file(RELATIVE_PATH _source_path_rel "${_src_hipe_server_inc_path}" "${_source_path}")
    string(REPLACE "/" "\\" _group_path "${_source_path_rel}")
    source_group("header\\${_group_path}" FILES "${_source}")
endforeach()



if(MSYS) #TODO: Is MSYS true when MSVC is true?
	message(STATUS "Test if it works with windows")
    target_link_libraries(hipe_server ws2_32 wsock32)
	target_link_libraries(hipe_server ws2_32 wsock32)

endif()



include_directories(hipe_server ${Hipe_INCLUDEDIR})
include_directories(hipe_server ${Live_INCLUDEDIR})
include_directories(hipe_server ${OpenCV_INCLUDE_DIRS} )
include_directories(hipe_server ${_src_hipe_server_inc_path})

message(STATUS "Live LIBRARIES : ${Live_LIBRARIES}")

target_link_libraries(hipe_server core)
target_link_libraries(hipe_server data)
target_link_libraries(hipe_server http)
target_link_libraries(hipe_server json)
target_link_libraries(hipe_server orchestrator)
##target_link_libraries(hipe_server streaming)




if (USE_DLIB)
	if (WIN32)
	target_link_libraries(hipe_server ${Dlib_LIBRARIES})

	#set( BLAS_openblas_LIBRARY ${HIPE_EXTERNAL}/OpenBLAS/)
	#find_package(BLAS REQUIRED)

	elseif (UNIX)
	target_link_libraries(hipe_server ${Dlib_LIBRARIES})
	endif()

endif()

message(STATUS "Library ffmpeg to include ${FFmpeg_LIBRARIES}")
message(STATUS "Library x264 to include ${x264_LIBRARIES}")
message(STATUS "Library Live to include ${Live_LIBRARIES}")
message(STATUS "Library OpenCV to include ${OpenCV_LIBS}")
message(STATUS "Library Boost to include ${Boost_LIBRARIES}")
target_link_libraries(hipe_server ${FFmpeg_LIBRARIES})
target_link_libraries(hipe_server ${x264_LIBRARIES})
target_link_libraries(hipe_server ${Live_LIBRARIES})
target_link_libraries(hipe_server ${OpenCV_LIBS})
target_link_libraries(hipe_server ${Boost_LIBRARIES})
target_link_libraries(hipe_server ${CMAKE_THREAD_LIBS_INIT})

if (USE_GPERFTOOLS)
  target_link_libraries(hipe_server ${GPERFTOOLS_DIR}/lib/libtcmalloc.a /usr/lib/aarch64-linux-gnu/libunwind.a /usr/lib/aarch64-linux-gnu/liblzma.a)
endif()



enable_testing()
#add_subdirectory(tests)

install(TARGETS hipe_server DESTINATION ../../target/)
#install(TARGETS hipe_server DESTINATION /usr/local/bin/)
