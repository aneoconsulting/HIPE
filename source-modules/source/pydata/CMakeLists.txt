include("${CMAKE_SOURCE_DIR}/cmake/common_functions_and_macros.cmake")
include("${CMAKE_SOURCE_DIR}/cmake/boost_config.cmake")

include("${CMAKE_SOURCE_DIR}/cmake/python27_config.cmake")
include("${CMAKE_SOURCE_DIR}/cmake/hipecore_config.cmake")

add_hipe_library(pydata SHARED)

target_compile_definitions(pydata PRIVATE "-DCV_VERSION_EPOCH=3" "-DUSE_BOOST_REGEX" "-DPYDATA_BUILD")

file(TO_CMAKE_PATH "$ENV{CUDA_PATH_V${CUDA_MAJOR}_${CUDA_MINOR}}" CUDA_TOOLKIT_ROOT_DIR)
message(STATUS "Set CUDA_TOOLKIT_ROOT_DIR : ${CUDA_TOOLKIT_ROOT_DIR}")

target_link_libraries(pydata ${hipecore_LIBRARIES})
if (WITH_CUBLAS OR WITH_CUDA)
	find_package(CUDA "8.0" EXACT REQUIRED)
	target_link_libraries(pydata ${CUDA_LIBRARIES})
	target_include_directories(pydata PRIVATE ${CUDA_INCLUDE_DIRS})
		
	set(USE_CUDA "-DUSE_CUDA")
	message(STATUS "CUBLAS IS INCLUDED ${CUDA_VERSION}")
endif()
include("${CMAKE_SOURCE_DIR}/cmake/opencv_config.cmake")
target_include_directories(pydata PUBLIC ${hipecore_INCLUDE_DIRS})
target_include_directories(pydata PRIVATE ${PYTHON27_INCLUDE_DIRS})


target_link_libraries(pydata ${Boost_LIBRARIES})
target_link_libraries(pydata ${CMAKE_THREAD_LIBS_INIT})
target_link_libraries(pydata ${OpenCV_LIBS})
target_link_libraries(pydata ${PYTHON27_LIBRARIES})


if(MSYS) #TODO: Is MSYS true when MSVC is true?
  message(STATUS "Test if it works with windows")
  target_link_libraries(pydata ws2_32 wsock32)
  target_link_libraries(pydata ws2_32 wsock32)
endif(MSYS)


add_custom_command(
  TARGET pydata
  POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:pydata> $<TARGET_FILE_DIR:pydata>/pydata.pyd
)

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
		
install (FILES $<TARGET_FILE_DIR:pydata>/pydata.pyd CONFIGURATIONS Release DESTINATION bin/Release COMPONENT library)
install (FILES $<TARGET_FILE_DIR:pydata>/pydata.pyd CONFIGURATIONS RelWithDebInfo DESTINATION bin/Release COMPONENT library)

install (FILES $<TARGET_FILE_DIR:pydata>/pydata.pyd CONFIGURATIONS Debug DESTINATION bin/Debug COMPONENT library)
