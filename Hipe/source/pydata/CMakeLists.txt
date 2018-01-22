include("${CMAKE_SOURCE_DIR}/cmake/common_functions_and_macros.cmake")
include("${CMAKE_SOURCE_DIR}/cmake/boost_config.cmake")
include("${CMAKE_SOURCE_DIR}/cmake/python27_config.cmake")

add_hipe_library(pydata)

target_compile_definitions(pydata PRIVATE "-DCV_VERSION_EPOCH=3" "-DUSE_BOOST_REGEX" "-DPYDATA_BUILD")

target_link_libraries(pydata core)
target_link_libraries(pydata ${OpenCV_LIBS})
target_link_libraries(pydata ${Boost_LIBRARIES})
target_link_libraries(pydata ${CMAKE_THREAD_LIBS_INIT})
target_link_libraries(pydata ${PYTHON27_LIBRARIES})

prepend_target_include_directories_if_necessary(pydata "${OpenCV_INCLUDE_DIRS}")
prepend_target_include_directories_if_necessary(pydata "${PYTHON27_INCLUDE_DIRS}")

if(MSYS) #TODO: Is MSYS true when MSVC is true?
  message(STATUS "Test if it works with windows")
  target_link_libraries(pydata ws2_32 wsock32)
  target_link_libraries(pydata ws2_32 wsock32)
endif(MSYS)


add_custom_command(TARGET pydata
                   POST_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:pydata> $<TARGET_FILE_DIR:pydata>/pydata.pyd)


display_target_properties(pydata)
