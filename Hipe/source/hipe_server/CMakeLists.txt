include("${CMAKE_SOURCE_DIR}/cmake/common_functions_and_macros.cmake")
include("${CMAKE_SOURCE_DIR}/cmake/boost_config.cmake")
include("${CMAKE_SOURCE_DIR}/cmake/opencv_config.cmake")
include(GetPrerequisites)

if(USE_DLIB)
  include("${CMAKE_SOURCE_DIR}/cmake/dlib_config.cmake")
endif(USE_DLIB)


#hipe_server LIBRARY
add_hipe_executable(hipe_server)

target_compile_definitions(hipe_server PRIVATE "-DUSE_BOOST_REGEX")


# TODO
# Is this not already included in the output of CMake? Remove?
display_pathlist("FFmpeg_LIBRARIES" "${FFmpeg_LIBRARIES}")
display_pathlist("x264_LIBRARIES" "${x264_LIBRARIES}")
display_pathlist("Live_LIBRARIES" "${Live_LIBRARIES}")
display_pathlist("OpenCV_LIBS" "${OpenCV_LIBS}")
display_pathlist("Boost_LIBRARIES" "${Boost_LIBRARIES}")
display_pathlist("Live_LIBRARIES" "${Live_LIBRARIES}")

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

if(USE_DLIB)
  target_link_libraries(hipe_server ${dlib_LIBRARIES})
  # TODO
  # Remove the previous line and use this without setting include_headers
  # when we finally start using proper dlib find_package support.
#   target_link_libraries(hipe_server dlib:dlib) #${dlib_LIBRARIES})
endif(USE_DLIB)

# TODO
# See notes abouve Dlib linking.
if(USE_DLIB)
endif(USE_DLIB)



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

#create directory for 3rdParty
install(DIRECTORY DESTINATION 3rdParty)
install(DIRECTORY DESTINATION "project")

install (SCRIPT "${CMAKE_SOURCE_DIR}/cmake/postInstall.cmake")

set(EXT_BIN "") 
if (WIN32)
set(EXT_BIN .exe)
endif() 

#file(TO_CMAKE_PATH "${CMAKE_INSTALL_PREFIX}/bin/Debug/hipe_server${EXT_BIN}" cm_path)
#file(TO_CMAKE_PATH "${CMAKE_INSTALL_PREFIX}/bin/Debug" cm_dir)
#set(PATH_SHAREDLIB "${HIPE_EXTERNAL_DIR}/boost_1_62_0/lib64-msvc-14.0;${HIPE_EXTERNAL_DIR}/opencv-3.4/x64/vc14/bin;${HIPE_EXTERNAL_DIR}/gstreamer/1.0/x86_64/bin;${cm_dir};${CMAKE_PREFIX_PATH}")
##STRING(REPLACE "\\" "\\\\" cm_path ${cm_path})
message(STATUS "Install dependencies exe")
install_dependencies_exe("hipe_server")

