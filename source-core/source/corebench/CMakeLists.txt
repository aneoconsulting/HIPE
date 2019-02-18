include("${CMAKE_SOURCE_DIR}/cmake/hipe_external_config.cmake")
include("${CMAKE_SOURCE_DIR}/cmake/common_functions_and_macros.cmake")
include("${CMAKE_SOURCE_DIR}/cmake/boost_config.cmake")
include("${CMAKE_SOURCE_DIR}/cmake/opencv_config.cmake")

add_hipe_library(corebench SHARED)

target_compile_definitions(corebench PRIVATE "-DUSE_BOOST_REGEX" "-DFILTER_BUILD")

include_directories(corebench ${OpenCV_INCLUDE_DIRS} )
target_link_libraries(corebench ${Boost_LIBRARIES})
target_link_libraries(corebench ${CMAKE_THREAD_LIBS_INIT})
target_link_libraries(corebench core)
target_link_libraries(corebench coredata)
target_link_libraries(corebench corefilter)
target_link_libraries(corebench json)
foreach(__component ${OpenCV_LIB_COMPONENTS})
target_link_libraries(corebench ${__component})
endforeach()

if(MSYS) #TODO: Is MSYS true when MSVC is true?
  message(STATUS "Test if it works with windows")
  target_link_libraries(binary ws2_32 wsock32)
  target_link_libraries(binary ws2_32 wsock32)
endif(MSYS)

display_target_properties(corebench)

install_header_to_target(corebench)

install (TARGETS corebench 
		CONFIGURATIONS Release 
		COMPONENT applications
		RUNTIME DESTINATION bin/Release 
		COMPONENT applications
		LIBRARY DESTINATION lib/Release
		COMPONENT applications
		ARCHIVE DESTINATION lib/Release
		COMPONENT library)
		
install (TARGETS corebench 
		CONFIGURATIONS Debug
		COMPONENT applications
		RUNTIME DESTINATION bin/Debug
		COMPONENT applications
		LIBRARY DESTINATION lib/Debug
		COMPONENT applications
		ARCHIVE DESTINATION lib/Debug
		COMPONENT library)

install (TARGETS corebench 
		CONFIGURATIONS RelWithDebInfo 
		COMPONENT applications
		RUNTIME DESTINATION bin/Release 
		COMPONENT applications
		LIBRARY DESTINATION lib/Release
		COMPONENT applications
		ARCHIVE DESTINATION lib/Release
		COMPONENT library)
