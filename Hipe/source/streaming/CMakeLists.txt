cmake_minimum_required (VERSION 3.7.1)

# include("${CMAKE_SOURCE_DIR}/cmake/hipe_external_config.cmake")
include("${CMAKE_SOURCE_DIR}/cmake/boost_config.cmake")
include("${CMAKE_SOURCE_DIR}/cmake/live_config.cmake")
include("${CMAKE_SOURCE_DIR}/cmake/ffmpeg_config.cmake")
include("${CMAKE_SOURCE_DIR}/cmake/x264_config.cmake")
include("${CMAKE_SOURCE_DIR}/cmake/sourcey_config.cmake")
set(Hipe_INCLUDEDIR "../../header/")

set(_src_streaming_path ${CMAKE_CURRENT_SOURCE_DIR})
message(STATUS "Get source path = ${_src_streaming_path}" )
file(
    GLOB_RECURSE _source_list
    LIST_DIRECTORIES false
    "${_src_streaming_path}/*.c*"
)

set(_src_streaming_inc_path ${CMAKE_SOURCE_DIR}/header/streaming)
file(
    GLOB_RECURSE _header_list
    LIST_DIRECTORIES false
    "${_src_streaming_inc_path}/*.h*"
)


##Include OpenCV
find_package( OpenCV REQUIRED )

##streaming LIBRARY

add_library(streaming STATIC ${_source_list} ${_header_list})
if (UNIX)
	target_compile_options(streaming PRIVATE -fPIC)
endif()

target_compile_definitions(streaming PRIVATE "-DUSE_BOOST_REGEX" "-DSTREAMING_BUILD" "-DSINGLETON_EXPORT")


target_link_libraries(streaming ${Boost_LIBRARIES})
target_link_libraries(streaming ${CMAKE_THREAD_LIBS_INIT})
target_link_libraries(streaming core)
target_link_libraries(streaming streaming_rtsp)
target_link_libraries(streaming ${OpenCV_LIBS})
target_link_libraries(streaming ${FFmpeg_LIBRARIES})
target_link_libraries(streaming ${x264_LIBRARIES})
target_link_libraries(streaming ${Live_LIBRARIES})


foreach(_source IN ITEMS ${_source_list})
	message(STATUS "item source = ${_source}")
    get_filename_component(_source_path "${_source}" PATH)
    file(RELATIVE_PATH _source_path_rel "${_src_streaming_path}" "${_source_path}")
    string(REPLACE "/" "\\" _group_path "${_source_path_rel}")

    source_group("source\\${_group_path}" FILES "${_source}")
endforeach()

foreach(_source IN ITEMS ${_header_list})
    get_filename_component(_source_path "${_source}" PATH)
    file(RELATIVE_PATH _source_path_rel "${_src_streaming_inc_path}" "${_source_path}")
    string(REPLACE "/" "\\" _group_path "${_source_path_rel}")
    source_group("header\\${_group_path}" FILES "${_source}")
endforeach()

include_directories(streaming ${Hipe_INCLUDEDIR})
include_directories(streaming ${OpenCV_INCLUDE_DIRS} )
include_directories(streaming ${FFmpeg_INCLUDEDIR} )
include_directories(streaming ${Live_INCLUDEDIR} )
include_directories(streaming ${x264_INCLUDEDIR} )
include_directories(streaming ${_src_streaming_inc_path})


if(WIN32) #TODO: Is MSYS true when MSVC is true?
	message(STATUS "Test if it works with windows")
     target_link_libraries(streaming ws2_32 wsock32 ws2_32 wsock32 Winmm Secur32 Msdmo Dmoguids	wmcodecdspuuid strmiids	Psapi Userenv)

endif()


install(TARGETS streaming DESTINATION ../../target/)
#install(TARGETS streaming DESTINATION /usr/local/lib/)
