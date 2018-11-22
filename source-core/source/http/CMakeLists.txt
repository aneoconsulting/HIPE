include("${CMAKE_SOURCE_DIR}/cmake/common_functions_and_macros.cmake")
include("${CMAKE_SOURCE_DIR}/cmake/boost_config.cmake")

#http LIBRARY

add_hipe_library(http STATIC)


message(STATUS "OpenSSL cmake dir ${OpenSSL_DIR}/lib/cmake")
find_package(BoringSSL 
				REQUIRED
				PATHS "${BoringSSL_DIR}/lib/cmake"
				NO_DEFAULT_PATH)

target_compile_definitions(http PRIVATE "-DUSE_BOOST_REGEX" "-DHTTP_BUILD")


target_link_libraries(http ${Boost_LIBRARIES})
target_link_libraries(http ${CMAKE_THREAD_LIBS_INIT})
target_link_libraries(http core)
target_link_libraries(http coredata)
target_link_libraries(http corefilter)
target_link_libraries(http json)
target_link_libraries(http orchestrator)
if (WIN32)
  target_link_libraries(http BoringSSL BoringCrypto)
  
else()
  target_compile_definitions(http PUBLIC "-DUNIX" "-DLINUX")
  target_link_libraries(http ssl crypto)
endif()




if(MSYS) #TODO: Is MSYS true when MSVC is true?
  message(STATUS "Test if it works with windows")
    target_link_libraries(binary ws2_32 wsock32)
  target_link_libraries(binary ws2_32 wsock32)
endif()

install_header_to_target(http)

install (TARGETS http CONFIGURATIONS Debug DESTINATION lib/Debug COMPONENT library)
install (TARGETS http CONFIGURATIONS Release DESTINATION lib/Release COMPONENT library)
install (TARGETS http CONFIGURATIONS RelWithDebInfo DESTINATION lib/Release COMPONENT library)
