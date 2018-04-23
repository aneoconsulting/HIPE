include("${CMAKE_SOURCE_DIR}/cmake/common_functions_and_macros.cmake")
include("${CMAKE_SOURCE_DIR}/cmake/boost_config.cmake")


add_hipe_library(core STATIC)


##Get BUILD from git file
file(STRINGS "${CMAKE_SOURCE_DIR}/BUILD_VERSION.txt" BUILD_VERSION)

target_compile_definitions(core PRIVATE "-DUSE_BOOST_REGEX" "-DCORE_BUILD" "-DBUILD_VERSION=${BUILD_VERSION}")

target_link_libraries(core ${Boost_LIBRARIES})
target_link_libraries(core ${CMAKE_THREAD_LIBS_INIT})



# FIXME
# What is this supposed to do? Should "binary" be "core"?
# Other files with similar blocks use different conditions while still
# mentioning MSYS. Should they check the same condition?

if(MSYS) #TODO: Is MSYS true when MSVC is true?
  message(STATUS "Test if it works with windows")
  target_link_libraries(binary ws2_32 wsock32)
  target_link_libraries(binary ws2_32 wsock32)
endif(MSYS)

display_target_properties(core)

install_header_to_target(core)

install (TARGETS core CONFIGURATIONS Debug DESTINATION lib/Debug)
install (TARGETS core CONFIGURATIONS Release DESTINATION lib/Release)
install (TARGETS core CONFIGURATIONS RelWithDebInfo DESTINATION lib/Release)
