include("${CMAKE_SOURCE_DIR}/cmake/common_functions_and_macros.cmake")
include("${CMAKE_SOURCE_DIR}/cmake/boost_config.cmake")
include("${CMAKE_SOURCE_DIR}/cmake/opencv_config.cmake")
include(GetPrerequisites)


#hipe_server LIBRARY
add_hipe_executable(hipe_server)

target_compile_definitions(hipe_server PRIVATE "-DUSE_BOOST_REGEX")


# TODO
# Is this not already included in the output of CMake? Remove?
display_pathlist("OpenCV_LIBS" "${OpenCV_LIBS}")
display_pathlist("Boost_LIBRARIES" "${Boost_LIBRARIES}")


target_link_libraries(hipe_server http)
target_link_libraries(hipe_server orchestrator)
target_link_libraries(hipe_server json)
target_link_libraries(hipe_server coredata)

target_link_libraries(hipe_server core)
target_link_libraries(hipe_server ${OpenCV_LIBS})
target_link_libraries(hipe_server ${Boost_LIBRARIES})
target_link_libraries(hipe_server ${CMAKE_THREAD_LIBS_INIT})

if (UNIX)
  target_link_libraries(hipe_server ${CMAKE_DL_LIBS})
endif()


if (MSVC)
target_link_libraries(hipe_server Imagehlp)
endif()

if(MSYS) #TODO: Is MSYS true when MSVC is true?
  message(STATUS "Test if it works with windows")
  target_link_libraries(hipe_server ws2_32 wsock32)
  target_link_libraries(hipe_server ws2_32 wsock32)
  
endif(MSYS)

if(USE_GPERFTOOLS)
  target_link_libraries(hipe_server ${GPERFTOOLS_DIR}/lib/libtcmalloc.a /usr/lib/aarch64-linux-gnu/libunwind.a /usr/lib/aarch64-linux-gnu/liblzma.a)
endif(USE_GPERFTOOLS)



enable_testing()
#add_subdirectory(tests)

display_target_properties(hipe_server)

install (TARGETS hipe_server CONFIGURATIONS Debug DESTINATION bin/Debug COMPONENT applications)
install (TARGETS hipe_server CONFIGURATIONS Release DESTINATION bin/Release COMPONENT applications)
install (TARGETS hipe_server CONFIGURATIONS RelWithDebInfo DESTINATION bin/Release COMPONENT applications)

#
#Copy in install folder all dependencies for hipe_core
#


set(EXT_BIN "") 
if (WIN32)
set(EXT_BIN .exe)
endif() 

message(STATUS "Install dependencies exe")
install_dependencies_exe("hipe_server")

install(CODE "set(PROJECT_SOURCE_DIR \"\${CMAKE_INSTALL_PREFIX}\")" COMPONENT applications)
install(CODE "set(LICENSE_DIR \"${PROJECT_SOURCE_DIR}\")" COMPONENT applications)
install(CODE "set(HIPE_INSTALL_DIR \"${CMAKE_INSTALL_PREFIX}\")" COMPONENT applications)
install (SCRIPT "${CMAKE_SOURCE_DIR}/cmake/postInstall.cmake" COMPONENT applications)

install(CODE "set(PROJECT_SOURCE_DIR \"\${CMAKE_INSTALL_PREFIX}/include\")" COMPONENT headers)
install(CODE "set(LICENSE_DIR \"${PROJECT_SOURCE_DIR}\")" COMPONENT headers)
install(CODE "set(HIPE_INSTALL_DIR \"${CMAKE_INSTALL_PREFIX}\")" COMPONENT headers)
install (SCRIPT "${CMAKE_SOURCE_DIR}/cmake/postInstall.cmake" COMPONENT headers)