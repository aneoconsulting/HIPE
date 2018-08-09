include("${CMAKE_SOURCE_DIR}/cmake/common_functions_and_macros.cmake")
include("${CMAKE_SOURCE_DIR}/cmake/boost_config.cmake")
include("${CMAKE_SOURCE_DIR}/cmake/opencv_config.cmake")

add_hipe_library(corefilter SHARED)

target_compile_definitions(corefilter PRIVATE "-DUSE_BOOST_REGEX" "-DFILTER_BUILD")

include_directories(corefilter ${OpenCV_INCLUDE_DIRS} )
target_link_libraries(corefilter ${Boost_LIBRARIES})
target_link_libraries(corefilter ${CMAKE_THREAD_LIBS_INIT})
target_link_libraries(corefilter core)
target_link_libraries(corefilter coredata)
target_link_libraries(corefilter json)



if(MSYS) #TODO: Is MSYS true when MSVC is true?
  message(STATUS "Test if it works with windows")
  target_link_libraries(binary ws2_32 wsock32)
  target_link_libraries(binary ws2_32 wsock32)
endif(MSYS)

display_target_properties(corefilter)

install_header_to_target(corefilter)

install (TARGETS corefilter 
		CONFIGURATIONS Release 
		COMPONENT applications
		RUNTIME DESTINATION bin/Release 
		COMPONENT applications
		LIBRARY DESTINATION lib/Release
		COMPONENT applications
		ARCHIVE DESTINATION lib/Release
		COMPONENT library)
		
install (TARGETS corefilter 
		CONFIGURATIONS Debug
		COMPONENT applications
		RUNTIME DESTINATION bin/Debug
		COMPONENT applications
		LIBRARY DESTINATION lib/Debug
		COMPONENT applications
		ARCHIVE DESTINATION lib/Debug
		COMPONENT library)

install (TARGETS corefilter 
		CONFIGURATIONS RelWithDebInfo 
		COMPONENT applications
		RUNTIME DESTINATION bin/Release 
		COMPONENT applications
		LIBRARY DESTINATION lib/Release
		COMPONENT applications
		ARCHIVE DESTINATION lib/Release
		COMPONENT library)