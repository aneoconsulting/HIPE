include("${CMAKE_SOURCE_DIR}/cmake/common_functions_and_macros.cmake")
include("${CMAKE_SOURCE_DIR}/cmake/boost_config.cmake")

add_hipe_library(json SHARED)

target_compile_definitions(json PRIVATE "-DUSE_BOOST_REGEX" "-DJSON_BUILD")

target_link_libraries(json ${Boost_LIBRARIES})
target_link_libraries(json ${CMAKE_THREAD_LIBS_INIT})
target_link_libraries(json core corefilter coredata)


if(MSYS) #TODO: Is MSYS true when MSVC is true?
  message(STATUS "Test if it works with windows")
    target_link_libraries(binary ws2_32 wsock32)
  target_link_libraries(binary ws2_32 wsock32)
endif(MSYS)

display_target_properties(json)


install_header_to_target(json)

install (TARGETS json 
		CONFIGURATIONS Release
		COMPONENT applications
		RUNTIME DESTINATION bin/Release 
		COMPONENT applications
		LIBRARY DESTINATION lib/Release
		COMPONENT applications
		ARCHIVE DESTINATION lib/Release
		COMPONENT library)
		
install (TARGETS json 
		CONFIGURATIONS Debug
		COMPONENT applications
		RUNTIME DESTINATION bin/Debug
		COMPONENT applications
		LIBRARY DESTINATION lib/Debug
		COMPONENT applications
		ARCHIVE DESTINATION lib/Debug
		COMPONENT library)

install (TARGETS json 
		CONFIGURATIONS RelWithDebInfo 
		COMPONENT applications
		RUNTIME DESTINATION bin/Release 
		COMPONENT applications
		LIBRARY DESTINATION lib/Release
		COMPONENT applications
		ARCHIVE DESTINATION lib/Release
		COMPONENT library)