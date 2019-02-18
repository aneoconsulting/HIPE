include("${CMAKE_SOURCE_DIR}/../../cmake/hipe_external_config.cmake")
include("${CMAKE_SOURCE_DIR}/../../cmake/common_functions_and_macros.cmake")
include("${CMAKE_SOURCE_DIR}/../../cmake/python36_config.cmake")
include("${CMAKE_SOURCE_DIR}/../../cmake/boost_config.cmake")


include("${CMAKE_SOURCE_DIR}/../../cmake/hipecore_config.cmake")

add_hipe_library(pydata full SHARED)

target_compile_definitions(pydata PUBLIC "$<$<CONFIG:Debug>:_DEBUG>")

if (WITH_GLOG)
	message(STATUS "Get Glog from ${GLOG_ROOT_DIR}")
	list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")
	find_package(Glog 
				REQUIRED
				PATHS "${GLOG_ROOT_DIR}/lib/cmake/glog"
				NO_DEFAULT_PATH)
	
	message(STATUS "Gflags include dir : ${GLOG_LIBRARIES}")
	message(STATUS "Gflags include dir : ${GLOG_LIBRARY}")
	set_target_properties(glog::glog PROPERTIES
	  MAP_IMPORTED_CONFIG_MINSIZEREL Release
	  MAP_IMPORTED_CONFIG_RELWITHDEBINFO Release
	  )
	target_include_directories(pydata PRIVATE ${GLOG_INCLUDE_DIR})
	target_link_libraries(pydata glog::glog)
	set(USE_GLOG "-DUSE_GLOG")
	message(STATUS "GLOG IS INCLUDED")

endif()


target_compile_definitions(pydata PRIVATE "-DCV_VERSION_EPOCH=3" "-DUSE_BOOST_REGEX" "-DPYDATA_BUILD")

target_link_libraries(pydata ${hipecore_LIBRARIES})
if (WITH_CUBLAS OR WITH_CUDA)
	find_package(CUDA "${CUDA_MAJOR}.${CUDA_MINOR}" EXACT REQUIRED)
	target_link_libraries(pydata ${CUDA_LIBRARIES})
	target_include_directories(pydata PRIVATE ${CUDA_INCLUDE_DIRS})
		
	set(USE_CUDA "-DUSE_CUDA")
	message(STATUS "CUBLAS IS INCLUDED ${CUDA_VERSION}")
endif()
include("${CMAKE_SOURCE_DIR}/../../cmake/opencv_config.cmake")
target_include_directories(pydata PUBLIC ${hipecore_INCLUDE_DIRS})
target_include_directories(pydata PRIVATE ${PYTHON36_INCLUDE_DIRS})


target_link_libraries(pydata ${Boost_LIBRARIES})
target_link_libraries(pydata ${CMAKE_THREAD_LIBS_INIT})
foreach(__component ${OpenCV_LIB_COMPONENTS})
	target_link_libraries(pydata ${__component})
endforeach()
target_link_libraries(pydata ${PYTHON36_LIBRARIES})


if(MSYS) #TODO: Is MSYS true when MSVC is true?
  message(STATUS "Test if it works with windows")
  target_link_libraries(pydata ws2_32 wsock32)
  target_link_libraries(pydata ws2_32 wsock32)
endif(MSYS)

if (WIN32)
  add_custom_command(
	TARGET pydata
	POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:pydata> $<TARGET_FILE_DIR:pydata>/pydata.pyd
	)
else()
  add_custom_command(
	TARGET pydata
	POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:pydata> $<TARGET_FILE_DIR:pydata>/pydata.so
	)
endif()

display_target_properties(pydata)

install (TARGETS pydata 
		CONFIGURATIONS Debug
		RUNTIME DESTINATION bin/Debug
		COMPONENT runtime
		LIBRARY DESTINATION lib/Debug
		COMPONENT library
		ARCHIVE DESTINATION lib/Debug
		COMPONENT library)

install (TARGETS pydata 
		CONFIGURATIONS Release 
		RUNTIME DESTINATION bin/Release 
		COMPONENT runtime
		LIBRARY DESTINATION lib/Release 
		COMPONENT library
		ARCHIVE DESTINATION lib/Release
		COMPONENT library)
		
install (TARGETS pydata 
		CONFIGURATIONS RelWithDebInfo 
		RUNTIME DESTINATION bin/Release 
		COMPONENT runtime
		LIBRARY DESTINATION lib/Release 
		COMPONENT library
		ARCHIVE DESTINATION lib/Release
		COMPONENT library)		

if (WIN32)
  SET(LIB_DIR "bin")
else()
  SET(LIB_DIR "lib")
endif()

install_dependencies(pydata)

if (WIN32)
install (FILES $<TARGET_FILE_DIR:pydata>/pydata.pyd CONFIGURATIONS Release DESTINATION ${LIB_DIR}/Release COMPONENT library)
install (FILES $<TARGET_FILE_DIR:pydata>/pydata.pyd CONFIGURATIONS RelWithDebInfo DESTINATION ${LIB_DIR}/Release COMPONENT library)

install (FILES $<TARGET_FILE_DIR:pydata>/pydata.pyd CONFIGURATIONS Debug DESTINATION ${LIB_DIR}/Debug COMPONENT library)
else()
install (FILES $<TARGET_FILE_DIR:pydata>/pydata.so CONFIGURATIONS Release DESTINATION ${LIB_DIR}/Release COMPONENT library)
install (FILES $<TARGET_FILE_DIR:pydata>/pydata.so CONFIGURATIONS RelWithDebInfo DESTINATION ${LIB_DIR}/Release COMPONENT library)

install (FILES $<TARGET_FILE_DIR:pydata>/pydata.so CONFIGURATIONS Debug DESTINATION ${LIB_DIR}/Debug COMPONENT library)
  
endif()
