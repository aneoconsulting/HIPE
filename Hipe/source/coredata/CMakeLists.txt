include("${CMAKE_SOURCE_DIR}/cmake/common_functions_and_macros.cmake")
include("${CMAKE_SOURCE_DIR}/cmake/boost_config.cmake")
include("${CMAKE_SOURCE_DIR}/cmake/opencv_config.cmake")
include("${CMAKE_SOURCE_DIR}/cmake/python27_config.cmake")

if(USE_DLIB)
  include("${CMAKE_SOURCE_DIR}/cmake/dlib_config.cmake")
endif(USE_DLIB)
#coredata LIBRARY

add_hipe_library(coredata SHARED)

if(USE_DLIB)
  target_link_libraries(coredata ${dlib_LIBRARIES})
  # TODO
  # Remove the previous line and use this without setting include_headers
  # when we finally start using proper dlib find_package support.
#   target_link_libraries(filter dlib:dlib) #${dlib_LIBRARIES})
endif(USE_DLIB)

target_compile_definitions(coredata PRIVATE "-DUSE_BOOST_REGEX" "-DDATA_BUILD")

target_link_libraries(coredata ${OpenCV_LIBS})
target_link_libraries(coredata ${Boost_LIBRARIES})
target_link_libraries(coredata ${CMAKE_THREAD_LIBS_INIT})
target_link_libraries(coredata ${PYTHON27_LIBRARIES})
# if (UNIX)
#   target_link_libraries(coredata '-Wl,--whole-archive core streaming -Wl,--no-whole-archive')
# endif(UNIX)
target_link_libraries(coredata core)


if(MSYS) #TODO: Is MSYS true when MSVC is true?
  message(STATUS "Test if it works with windows")
  target_link_libraries(binary ws2_32 wsock32)
  target_link_libraries(binary ws2_32 wsock32)
endif(MSYS)

display_target_properties(coredata)

install_header_to_target(coredata)

install (TARGETS coredata 
		CONFIGURATIONS Release 
		COMPONENT applications
		RUNTIME DESTINATION bin/Release 
		COMPONENT applications
		LIBRARY DESTINATION lib/Release
		COMPONENT applications
		ARCHIVE DESTINATION lib/Release
		COMPONENT library)
		
install (TARGETS coredata 
		CONFIGURATIONS Debug
		COMPONENT applications
		RUNTIME DESTINATION bin/Debug
		COMPONENT applications
		LIBRARY DESTINATION lib/Debug
		COMPONENT applications
		ARCHIVE DESTINATION lib/Debug
		COMPONENT library)
		
install (TARGETS coredata 
		CONFIGURATIONS RelWithDebInfo
		COMPONENT applications
		RUNTIME DESTINATION bin/Release 
		COMPONENT applications
		LIBRARY DESTINATION lib/Release
		ARCHIVE DESTINATION lib/Release
		COMPONENT library)
		
