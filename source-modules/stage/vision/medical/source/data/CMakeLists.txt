include("${CMAKE_SOURCE_DIR}/../../cmake/hipe_external_config.cmake")
include("${CMAKE_SOURCE_DIR}/../../cmake/common_functions_and_macros.cmake")
include("${CMAKE_SOURCE_DIR}/../../cmake/boost_config.cmake")
include("${CMAKE_SOURCE_DIR}/../../cmake/opencv_config.cmake")
include("${CMAKE_SOURCE_DIR}/../../cmake/python36_config.cmake")
include("${CMAKE_SOURCE_DIR}/../../cmake/hipecore_config.cmake")

message(STATUS "DLIB_DIR ${Dlib_DIR}")
if(USE_DLIB)
  include("${CMAKE_SOURCE_DIR}/../../cmake/dlib_config.cmake")
endif(USE_DLIB)
#data LIBRARY

add_hipe_library(data full SHARED)

target_compile_definitions(data PUBLIC "$<$<CONFIG:Debug>:_DEBUG>")


if(USE_DLIB)
  message(STATUS "USE_DLIB : INLCUDE DIR : ${dlib_INCLUDE_DIRS}")
  target_include_directories(data PUBLIC ${dlib_INCLUDE_DIRS})
  target_link_libraries(data ${dlib_LIBRARIES})
  # TODO
  # Remove the previous line and use this without setting include_headers
  # when we finally start using proper dlib find_package support.
#   target_link_libraries(filter dlib:dlib) #${dlib_LIBRARIES})
endif(USE_DLIB)

target_compile_definitions(data PRIVATE "-DUSE_BOOST_REGEX" "-DDATA_BUILD")

target_link_libraries(data ${hipecore_LIBRARIES})
target_include_directories(data PUBLIC ${hipecore_INCLUDE_DIRS})
foreach(__component ${OpenCV_LIB_COMPONENTS})
target_link_libraries(data ${__component})
endforeach()
target_link_libraries(data ${Boost_LIBRARIES})
target_link_libraries(data ${CMAKE_THREAD_LIBS_INIT})
target_link_libraries(data ${PYTHON36_LIBRARIES})
# if (UNIX)
#   target_link_libraries(data '-Wl,--whole-archive core streaming -Wl,--no-whole-archive')
# endif(UNIX)



if(MSYS) #TODO: Is MSYS true when MSVC is true?
  message(STATUS "Test if it works with windows")
  target_link_libraries(binary ws2_32 wsock32)
  target_link_libraries(binary ws2_32 wsock32)
endif(MSYS)

display_target_properties(data)

install (TARGETS data 
		CONFIGURATIONS Debug
		RUNTIME DESTINATION bin/Debug
		COMPONENT runtime
		LIBRARY DESTINATION lib/Debug
		COMPONENT library
		ARCHIVE DESTINATION lib/Debug
		COMPONENT library)

install (TARGETS data 
		CONFIGURATIONS Release 
		RUNTIME DESTINATION bin/Release 
		COMPONENT runtime
		LIBRARY DESTINATION lib/Release
		COMPONENT library
		ARCHIVE DESTINATION lib/Release
		COMPONENT library)
		
install (TARGETS data 
		CONFIGURATIONS RelWithDebInfo 
		RUNTIME DESTINATION bin/Release 
		COMPONENT runtime
		LIBRARY DESTINATION lib/Release 
		COMPONENT library
		ARCHIVE DESTINATION lib/Release
		COMPONENT library)
