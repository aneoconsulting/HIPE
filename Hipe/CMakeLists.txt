cmake_minimum_required (VERSION 3.7.1)
project (Hipe)



# Options
option(USE_GPERFTOOLS "Use gperftools." OFF)
option(USE_DLIB "Enable dlib library." ON)
# option(HIPE_STATIC_LIBS "Compile static libraries." ON)
option(HIPE_STATIC_EXES "Compile static executables." OFF)

option(HIPE_EXTERNAL_PYTHON27 "Use Python 2.7 libraries in Hipe External." ON)
message(STATUS "HIPE_EXTERNAL_PYTHON27: ${HIPE_EXTERNAL_PYTHON27}")

option(HIPE_EXTERNAL_BOOST "Use Boost libraries in Hipe External." ON)
message(STATUS "HIPE_EXTERNAL_BOOST: ${HIPE_EXTERNAL_BOOST}")

option(HIPE_EXTERNAL_OPENCV "Use OpenCV libraries in Hipe External." ON)
message(STATUS "HIPE_EXTERNAL_OPENCV: ${HIPE_EXTERNAL_OPENCV}")


# Dlib is not supported by find_package, so "Dlib_DIR" must be set to the installation path.
# option(HIPE_EXTERNAL_DLIB "Use Dlib libraries in Hipe External." ON)
# message(STATUS "HIPE_EXTERNAL_DLIB: ${HIPE_EXTERNAL_DLIB}")

if (WIN32)
option(CMAKE_USE_WIN32_THREADS_INIT "using WIN32 threads" ON)
endif(WIN32)
# x264 is not supported by find_package, so "x264_DIR" must be set to the installation path.
# option(HIPE_EXTERNAL_X264 "Use x264 libraries in Hipe External." ON)

# LiveMedia is not supported by find_package, so "LiveMedia_DIR" must be set to the installation path.
# option(HIPE_EXTERNAL_LIVEMEDIA "Use Live-Media libraries in Hipe External." ON)

# FFmpeg is not supported by find_package, so "FFmpeg_DIR" must be set to the installation path.
# option(HIPE_EXTERNAL_FFMPEG "Use FFmpeg libraries in Hipe External." ON)

# Configure common settings.
include("${CMAKE_SOURCE_DIR}/cmake/common_config.cmake")

#controls where to build the executables after "make"
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/target)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/target/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/target/libs)
set(CMAKE_INCLUDE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/target/include)

IF(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
  SET(CMAKE_INSTALL_PREFIX "${CMAKE_BINARY_DIR}/install" CACHE PATH "Installation directory" FORCE)
ENDIF(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
add_subdirectory(source)



# add_subdirectory(tests)
