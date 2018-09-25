include("${CMAKE_SOURCE_DIR}/cmake/common_functions_and_macros.cmake")
include("${CMAKE_SOURCE_DIR}/cmake/boost_config.cmake")
include("${CMAKE_SOURCE_DIR}/cmake/opencv_config.cmake")
include(GetPrerequisites)


#hipe_server LIBRARY
add_hipe_executable(hipe_server)

target_compile_definitions(hipe_server PRIVATE "-DUSE_BOOST_REGEX")

if (WITH_GFLAGS)
		message(STATUS "Get Gflags from ${GFLAGS_ROOT_DIR}")
		find_package(Gflags 
					REQUIRED
					PATHS "${GFLAGS_ROOT_DIR}/lib/cmake/gflags"
					NO_DEFAULT_PATH)

				  message(STATUS "Gflags include dir : ${GFLAGS_INCLUDE_DIR}")
		if (WIN32)
		  target_link_libraries(hipe_server ${GFLAGS_LIBRARIES})
		else()
		  target_link_libraries(hipe_server gflags_shared)
		endif()
		target_include_directories(hipe_server PRIVATE ${GFLAGS_INCLUDE_DIRS})
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
	
	message(STATUS "Gflags include dir : ${GLOG_LIBRARIES}")
	message(STATUS "Gflags include dir : ${GLOG_LIBRARY}")

	target_include_directories(hipe_server PRIVATE ${GLOG_INCLUDE_DIR})
	target_link_libraries(hipe_server glog::glog)
	set(USE_GLOG "-DUSE_GLOG")
	message(STATUS "GLOG IS INCLUDED")

endif()

# TODO
# Is this not already included in the output of CMake? Remove?
display_pathlist("OpenCV_LIBS" "${OpenCV_LIBS}")
display_pathlist("Boost_LIBRARIES" "${Boost_LIBRARIES}")
message(STATUS "OpenSSL cmake dir ${OpenSSL_DIR}/lib/cmake")
find_package(BoringSSL 
				REQUIRED
				PATHS "${BoringSSL_DIR}/lib/cmake"
				NO_DEFAULT_PATH)


target_link_libraries(hipe_server http)
target_link_libraries(hipe_server orchestrator)
target_link_libraries(hipe_server json)
target_link_libraries(hipe_server coredata)
target_link_libraries(hipe_server corefilter)
target_link_libraries(hipe_server corebench)

target_link_libraries(hipe_server core)
target_link_libraries(hipe_server ${OpenCV_LIBS})
target_link_libraries(hipe_server ${Boost_LIBRARIES})
target_link_libraries(hipe_server ${CMAKE_THREAD_LIBS_INIT})

if (WIN32)
  target_link_libraries(hipe_server BoringSSL BoringCrypto)
else()
  target_link_libraries(hipe_server ssl crypto)
endif()

if (UNIX)
  target_link_libraries(hipe_server ${CMAKE_DL_LIBS})
endif()


if (MSVC)
target_link_libraries(hipe_server Imagehlp)
endif()

if(MSYS) #TODO: Is MSYS true when MSVC is true?
  message(STATUS "Test if it works with windows")
  target_link_libraries(hipe_server ws2_32 wsock32)
  target_link_libraries(hipe_server ws2_32 wsock32)
  
endif(MSYS)

if(USE_GPERFTOOLS)
  target_link_libraries(hipe_server ${GPERFTOOLS_DIR}/lib/libtcmalloc.a /usr/lib/aarch64-linux-gnu/libunwind.a /usr/lib/aarch64-linux-gnu/liblzma.a)
endif(USE_GPERFTOOLS)



enable_testing()
#add_subdirectory(tests)

display_target_properties(hipe_server)

install (TARGETS hipe_server CONFIGURATIONS Debug DESTINATION bin/Debug COMPONENT applications)
install (TARGETS hipe_server CONFIGURATIONS Release DESTINATION bin/Release COMPONENT applications)
install (TARGETS hipe_server CONFIGURATIONS RelWithDebInfo DESTINATION bin/Release COMPONENT applications)

#
#Copy in install folder all dependencies for hipe_core
#


set(EXT_BIN "") 
if (WIN32)
set(EXT_BIN .exe)
endif() 

message(STATUS "Install dependencies exe")
install_dependencies_exe("hipe_server")

install(CODE "set(PROJECT_SOURCE_DIR \"\${CMAKE_INSTALL_PREFIX}\")" COMPONENT applications)
install(CODE "set(LICENSE_DIR \"${PROJECT_SOURCE_DIR}\")" COMPONENT applications)
install(CODE "set(HIPE_INSTALL_DIR \"${CMAKE_INSTALL_PREFIX}\")" COMPONENT applications)
install (SCRIPT "${CMAKE_SOURCE_DIR}/cmake/postInstall.cmake" COMPONENT applications)

install(CODE "set(PROJECT_SOURCE_DIR \"\${CMAKE_INSTALL_PREFIX}/include\")" COMPONENT headers)
install(CODE "set(LICENSE_DIR \"${PROJECT_SOURCE_DIR}\")" COMPONENT headers)
install(CODE "set(HIPE_INSTALL_DIR \"${CMAKE_INSTALL_PREFIX}\")" COMPONENT headers)
install (SCRIPT "${CMAKE_SOURCE_DIR}/cmake/postInstall.cmake" COMPONENT headers)
