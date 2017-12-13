cmake_minimum_required (VERSION 3.7.1)


set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wall")

include_directories(.)

find_package(Threads REQUIRED)

set(BOOST_COMPONENTS system thread filesystem date_time regex log program_options python)

set(BOOST_LIBRARYDIR "BOOST-LIBRARY-NOTFOUND" CACHE PATH "Path to the static Boost Library" PARENT_SCOPE)

# Late 2017 TODO: remove the following checks and always use std::regex
if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    if (CMAKE_CXX_COMPILER_VERSION VERSION_LESS 4.9)
        set(BOOST_COMPONENTS ${BOOST_COMPONENTS} regex)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DUSE_BOOST_REGEX")
    endif()
endif()

#add_definitions(-DBOOST_ALL_NO_LIB)
ADD_DEFINITIONS(-DBOOST_ALL_DYN_LINK)
if (WIN32)
	ADD_DEFINITIONS(-DBOOST_USE_WINAPI_VERSION=0x601)
	
endif()

set(Boost_USE_STATIC_LIBS       OFF) # only find static libs
set(Boost_USE_MULTITHREADED      ON)
set(Boost_USE_STATIC_RUNTIME    OFF)

find_package(Boost 1.62.0 REQUIRED COMPONENTS ${BOOST_COMPONENTS} )
if(${Boost_FOUND})
	include_directories(SYSTEM ${Boost_INCLUDE_DIR})
endif()	

get_directory_property(_my_link_dirs LINK_DIRECTORIES)
message(STATUS "_my_link_dirs = ${_my_link_dirs}") 
