include("${CMAKE_SOURCE_DIR}/cmake/hipe_external_config.cmake")
include("${CMAKE_SOURCE_DIR}/cmake/common_functions_and_macros.cmake")
include("${CMAKE_SOURCE_DIR}/cmake/boost_config.cmake")
include("${CMAKE_SOURCE_DIR}/cmake/opencv_config.cmake")
include(GetPrerequisites)


#hipe_engine LIBRARY
add_hipe_executable(hipe_engine)

target_compile_definitions(hipe_engine PRIVATE "-DUSE_BOOST_REGEX")
target_compile_definitions(hipe_engine PUBLIC "$<$<CONFIG:Debug>:_DEBUG>")

if (WITH_GFLAGS)
		message(STATUS "Get Gflags from ${GFLAGS_ROOT_DIR}")
		find_package(Gflags 
					REQUIRED
					PATHS "${GFLAGS_ROOT_DIR}/lib/cmake/gflags"
					NO_DEFAULT_PATH)
	  message(STATUS "Gflags include dir : ${GFLAGS_INCLUDE_DIR}")
	  if (WIN32)
		set_target_properties(gflags_static PROPERTIES
		  MAP_IMPORTED_CONFIG_MINSIZEREL Release
		  MAP_IMPORTED_CONFIG_RELWITHDEBINFO Release
		  )
		
		target_link_libraries(hipe_engine gflags_static)
	  else()
		  target_link_libraries(hipe_engine gflags_shared)
		endif()
		target_include_directories(hipe_engine PRIVATE ${GFLAGS_INCLUDE_DIRS})
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
	#set_target_properties(glog::glog PROPERTIES
	#  MAP_IMPORTED_CONFIG_MINSIZEREL Release
	#  MAP_IMPORTED_CONFIG_RELWITHDEBINFO Release
	#  )
	message(STATUS "Gflags include dir : ${GLOG_LIBRARIES}")
	message(STATUS "Gflags include dir : ${GLOG_LIBRARY}")

	target_include_directories(hipe_engine PRIVATE ${GLOG_INCLUDE_DIR})
	target_link_libraries(hipe_engine glog::glog)
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

target_include_directories(hipe_engine PRIVATE ${HIPE_EXTERNAL_DIR})
target_include_directories(hipe_engine PRIVATE ${HIPE_EXTERNAL_DIR}/include)

#target_link_libraries(hipe_engine http)
target_link_libraries(hipe_engine orchestrator)
target_link_libraries(hipe_engine json)
target_link_libraries(hipe_engine coredata)
target_link_libraries(hipe_engine corefilter)
target_link_libraries(hipe_engine corebench)

target_link_libraries(hipe_engine core)
foreach(__component ${OpenCV_LIB_COMPONENTS})
target_link_libraries(hipe_engine ${__component})
endforeach()
target_link_libraries(hipe_engine ${Boost_LIBRARIES})
target_link_libraries(hipe_engine ${CMAKE_THREAD_LIBS_INIT})

if (WIN32)
  target_link_libraries(hipe_engine BoringSSL BoringCrypto)
else()
  target_link_libraries(hipe_engine ssl crypto)
  target_link_libraries(hipe_engine -lrt)
endif()

if (UNIX)
  target_link_libraries(hipe_engine ${CMAKE_DL_LIBS})
endif()


if (MSVC)
target_link_libraries(hipe_engine Imagehlp)
endif()

if(MSYS) #TODO: Is MSYS true when MSVC is true?
  message(STATUS "Test if it works with windows")
  target_link_libraries(hipe_engine ws2_32 wsock32)
  target_link_libraries(hipe_engine ws2_32 wsock32)
  
endif(MSYS)

if(USE_GPERFTOOLS)
  target_link_libraries(hipe_engine ${GPERFTOOLS_DIR}/lib/libtcmalloc.a /usr/lib/aarch64-linux-gnu/libunwind.a /usr/lib/aarch64-linux-gnu/liblzma.a)
endif(USE_GPERFTOOLS)



enable_testing()
#add_subdirectory(tests)

display_target_properties(hipe_engine)

install (TARGETS hipe_engine CONFIGURATIONS Debug DESTINATION bin/Debug COMPONENT applications)
install (TARGETS hipe_engine CONFIGURATIONS Release DESTINATION bin/Release COMPONENT applications)
install (TARGETS hipe_engine CONFIGURATIONS RelWithDebInfo DESTINATION bin/Release COMPONENT applications)

#
#Copy in install folder all dependencies for hipe_core
#


set(EXT_BIN "") 
if (WIN32)
set(EXT_BIN .exe)
endif() 

message(STATUS "Install dependencies exe")
install_dependencies_exe("hipe_engine")

install(CODE "set(PROJECT_SOURCE_DIR \"\${CMAKE_INSTALL_PREFIX}\")" COMPONENT applications)
install(CODE "set(LICENSE_DIR \"${PROJECT_SOURCE_DIR}\")" COMPONENT applications)
install(CODE "set(HIPE_INSTALL_DIR \"${CMAKE_INSTALL_PREFIX}\")" COMPONENT applications)
install (SCRIPT "${CMAKE_SOURCE_DIR}/cmake/postInstall.cmake" COMPONENT applications)

install(CODE "set(PROJECT_SOURCE_DIR \"\${CMAKE_INSTALL_PREFIX}/include\")" COMPONENT headers)
install(CODE "set(LICENSE_DIR \"${PROJECT_SOURCE_DIR}\")" COMPONENT headers)
install(CODE "set(HIPE_INSTALL_DIR \"${CMAKE_INSTALL_PREFIX}\")" COMPONENT headers)
install (SCRIPT "${CMAKE_SOURCE_DIR}/cmake/postInstall.cmake" COMPONENT headers)
