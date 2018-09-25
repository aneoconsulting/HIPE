include("${CMAKE_SOURCE_DIR}/cmake/common_functions_and_macros.cmake")
include("${CMAKE_SOURCE_DIR}/cmake/boost_config.cmake")

set(USE_GFLAGS "")
set(USE_GLOG "")



add_hipe_library(core STATIC)


if (WITH_GFLAGS)
		message(STATUS "Get Gflags from ${GFLAGS_ROOT_DIR}")
		find_package(Gflags 
					REQUIRED
					PATHS "${GFLAGS_ROOT_DIR}/lib/cmake/gflags"
					NO_DEFAULT_PATH)

				  message(STATUS "Gflags include dir : ${GFLAGS_INCLUDE_DIR}")
		if (WIN32)
		  target_link_libraries(core ${GFLAGS_LIBRARIES})
		else()
		  target_link_libraries(core gflags_shared)
		endif()
		target_include_directories(core PRIVATE ${GFLAGS_INCLUDE_DIRS})
		set(USE_GFLAGS "-DUSE_GFLAGS")
		message(STATUS "GFLAGS IS INCLUDED")
endif()

if (WITH_GLOG)
	message(STATUS "Get Glog from ${GLOG_ROOT_DIR}")
	list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")
	find_package(Glog 
				REQUIRED
				PATHS "${GLOG_ROOT_DIR}/lib/cmake/glog"
				NO_DEFAULT_PATH)
	
	message(STATUS "Glog include dir : ${GLOG_LIBRARIES}")
	message(STATUS "Glog include dir : ${GLOG_LIBRARY}")
	target_include_directories(core PRIVATE ${GLOG_INCLUDE_DIR})
	target_link_libraries(core glog::glog)
	set(USE_GLOG "-DUSE_GLOG")
	message(STATUS "GLOG IS INCLUDED")

endif()


##Get BUILD from git file
file(STRINGS "${CMAKE_SOURCE_DIR}/BUILD_VERSION.txt" BUILD_VERSION)

target_compile_definitions(core PRIVATE "-DUSE_BOOST_REGEX" "-DCORE_BUILD" "-DBUILD_VERSION=${BUILD_VERSION}")

target_link_libraries(core ${Boost_LIBRARIES})
target_link_libraries(core ${CMAKE_THREAD_LIBS_INIT})



# FIXME
# What is this supposed to do? Should "binary" be "core"?
# Other files with similar blocks use different conditions while still
# mentioning MSYS. Should they check the same condition?

if(MSYS) #TODO: Is MSYS true when MSVC is true?
  message(STATUS "Test if it works with windows")
  target_link_libraries(binary ws2_32 wsock32)
  target_link_libraries(binary ws2_32 wsock32)
endif(MSYS)

display_target_properties(core)

install_header_to_target(core)

install (TARGETS core CONFIGURATIONS Debug DESTINATION lib/Debug)
install (TARGETS core CONFIGURATIONS Release DESTINATION lib/Release)
install (TARGETS core CONFIGURATIONS RelWithDebInfo DESTINATION lib/Release)
