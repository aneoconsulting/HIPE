cmake_minimum_required (VERSION 3.7.1)


include("${CMAKE_SOURCE_DIR}/boost_config.cmake")
set(Hype_INCLUDEDIR "../../header/")

set(_src_exception_path ${CMAKE_CURRENT_SOURCE_DIR})
file(
    GLOB_RECURSE _source_list 
    LIST_DIRECTORIES false
    "${_src_exception_path}/*.c*"
)

set(_src_exception_inc_path ${CMAKE_SOURCE_DIR}/header/exception)
file(
    GLOB_RECURSE _header_list 
    LIST_DIRECTORIES false
    "${_src_exception_inc_path}/*.h*"
)

