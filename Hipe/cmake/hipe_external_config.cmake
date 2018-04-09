cmake_minimum_required(VERSION 3.7.1)

# Check if HIPE_EXTERNAL has been set via the command-line via
#
# -DHIPE_EXTERNAL=<path>
#
# If not, check if the environment variable of the same name has been set and
# use that instead. If neither has been set, raise a fatal error.

# Unset via command-line.
if(NOT DEFINED HIPE_EXTERNAL)
  # Set via environment variable.
  if(DEFINED ENV{HIPE_EXTERNAL})
    set(HIPE_EXTERNAL "$ENV{HIPE_EXTERNAL}")
  # Unset via environment variable.
  else(DEFINED ENV{HIPE_EXTERNAL})
    message(FATAL_ERROR "Could not find \"HIPE_EXTERNAL\". Please set it locally or by environment variable.")
  endif(DEFINED ENV{HIPE_EXTERNAL})
endif(NOT DEFINED HIPE_EXTERNAL)

# Make sure that it is an absolute path.
get_filename_component(HIPE_EXTERNAL "${HIPE_EXTERNAL}" ABSOLUTE)

# Inform the user which value was used.
message(STATUS "HIPE_EXTERNAL : ${HIPE_EXTERNAL}")

if (NOT (${USE_PREBUILT_DEPENDENCIES} OR ${FORCE_USE_PREBUILT_DEPENDENCIES}) )
	# Make sure HIPE_EXTERNAL exists.
	if(NOT EXISTS "${HIPE_EXTERNAL}")
	  message(FATAL_ERROR "${HIPE_EXTERNAL} does not exist")
	elseif(NOT IS_DIRECTORY "${HIPE_EXTERNAL}")
	  message(FATAL_ERROR "${HIPE_EXTERNAL} is not a directory")
	endif(NOT EXISTS "${HIPE_EXTERNAL}")
else()
	file(MAKE_DIRECTORY "${HIPE_EXTERNAL}")
	if (WIN32)
		file(MAKE_DIRECTORY "${HIPE_EXTERNAL}/win64")
	elseif(LINUX)
		file(MAKE_DIRECTORY "${HIPE_EXTERNAL}/linux64/install")
	endif()
endif()



if(WIN32)
  set(HIPE_EXTERNAL_DIR "${HIPE_EXTERNAL}/win64")
else(WIN32)
  set(HIPE_EXTERNAL_DIR "${HIPE_EXTERNAL}/linux64/install")
  # Without this, the linker fails to find e.g. libboost_wave when building
  # multiple libraries ("cannot find -lboost_wave"), despite correctly locating
  # the libraries in HIPE_EXTERNAL with the find_package command. For example,
  # building streaming_rtsp or core separately succeeds but building both at the
  # same time results in the aforementioned error.
#   link_libraries("-L '${HIPE_EXTERNAL_DIR}/lib'")
endif(WIN32)

if(WIN32)
  # TODO
  # Update this section to enable optional use of installed system libraries.
  set(Boost_DIR "${HIPE_EXTERNAL_DIR}/boost_1_62_0/" CACHE PATH "Boost_DIR" FORCE)
  set(Boost_INCLUDE_DIR "${Boost_DIR}" CACHE PATH "Boost_INCLUDE_DIR" FORCE)
  set(BOOST_LIBRARYDIR "${Boost_DIR}/lib64-msvc-14.0" CACHE PATH "BOOST_LIBRARYDIR" FORCE)
  set(BOOST_ROOT "${Boost_DIR}/"  CACHE PATH "BOOST_ROOT" FORCE)
  if(HIPE_EXTERNAL_BOOST)
    list(APPEND CMAKE_PREFIX_PATH "${Boost_DIR}")
  endif(HIPE_EXTERNAL_BOOST)

  set(OpenCV_DIR "${HIPE_EXTERNAL_DIR}/opencv-3.4" CACHE PATH "OpenCV Directory" FORCE)
  if(HIPE_EXTERNAL_OPENCV)
    list(APPEND CMAKE_PREFIX_PATH "${OpenCV_DIR}")
  endif(HIPE_EXTERNAL_OPENCV)

  set(x264_DIR "${HIPE_EXTERNAL_DIR}/x264-devel-win64" CACHE PATH "x264" FORCE)

  set(LiveMedia_DIR "${HIPE_EXTERNAL_DIR}/live555/install" CACHE PATH "LiveMedia_DIR" FORCE)

  set(FFmpeg_DIR "${HIPE_EXTERNAL_DIR}/ffmpeg" CACHE PATH "FFMPEG_LIBRARYDIR" FORCE)

  set(Sourcey_DIR "${HIPE_EXTERNAL_DIR}/libsourcey" CACHE PATH "SOURCEY_LIBRARYDIR" FORCE)

  set(WebRTC_DIR "${HIPE_EXTERNAL_DIR}/webrtc" CACHE PATH "SOURCEY_LIBRARYDIR" FORCE)

  set(UV_DIR "${HIPE_EXTERNAL_DIR}/libuv"  CACHE PATH "LIBUV_LIBRARYDIR" FORCE)

  set(Dlib_DIR "${HIPE_EXTERNAL_DIR}/dlib-19.8/"  CACHE PATH "DLIB_LIBRARYDIR" FORCE)

  set(Python27_DIR "${HIPE_EXTERNAL_DIR}/Python2.7.14"  CACHE PATH "PYTHON_LIBRARYDIR" FORCE )
  if(HIPE_EXTERNAL_PYTHON27)
    list(APPEND CMAKE_PREFIX_PATH "${Python27_DIR}")
  endif(HIPE_EXTERNAL_PYTHON27)

  #set(YOLOV2_DIR "${HIPE_EXTERNAL_DIR}/yolov2/"  CACHE PATH "YOLOV2_LIBRARYDIR" FORCE )
  set(OpenBLAS_DIR "${HIPE_EXTERNAL_DIR}/OpenBLAS")

else(WIN32)
#   set(Python27_DIR "${HIPE_EXTERNAL_DIR}/python27/usr"  CACHE PATH "PYTHON_LIBRARYDIR")
#   set(PYTHON_LIBRARY "${Python27_DIR}/lib" CACHE PATH "PYTHON_LIBRARY")
#   set(PYTHON_INCLUDE_DIR "${Python27_DIR}/include/python2.7" CACHE PATH "PYTHON_INCLUDE_DIR")
if(HIPE_EXTERNAL_PYTHON27)
  set(Python27_DIR "${HIPE_EXTERNAL_DIR}/python27/usr"  CACHE PATH "PYTHON_LIBRARYDIR")
  message(STATUS "HIPE_PYTHON27 is activated : ${HIPE_EXTERNAL_DIR}")
  list(APPEND CMAKE_PREFIX_PATH "${HIPE_EXTERNAL_DIR}/python27/usr")
endif(HIPE_EXTERNAL_PYTHON27)

#   set(OpenCV_DIR "${HIPE_EXTERNAL_DIR}/opencv/share/OpenCV" CACHE PATH "OpenCV")
  if(HIPE_EXTERNAL_OPENCV)
    list(APPEND CMAKE_PREFIX_PATH "${HIPE_EXTERNAL_DIR}/cuda8")
    set(ENV{PATH} "${HIPE_EXTERNAL_DIR}/cuda8/bin:$ENV{PATH}")
    list(APPEND CMAKE_PREFIX_PATH "${HIPE_EXTERNAL_DIR}/opencv")
  endif(HIPE_EXTERNAL_OPENCV)

#   # Set Boost hints used by FindBoost.cmake to find the boost libraries.
#   set(BOOST_ROOT "${HIPE_EXTERNAL_DIR}/boost" CACHE PATH "BOOST_ROOT")
#   set(Boost_INCLUDE_DIR "${BOOST_ROOT}/include" CACHE PATH "BOOST_INCLUDEDIR" FORCE)
#   set(BOOST_LIBRARYDIR "${BOOST_ROOT}/lib" CACHE PATH "BOOST_LIBRARYDIR" FORCE)
  if(HIPE_EXTERNAL_BOOST)
    list(APPEND CMAKE_PREFIX_PATH "${HIPE_EXTERNAL_DIR}/boost")
  endif(HIPE_EXTERNAL_BOOST)
  

  set(x264_DIR "${HIPE_EXTERNAL_DIR}/ffmpeg" CACHE PATH "x264")
  list(APPEND CMAKE_PREFIX_PATH "${x264_DIR}")
  
  set(LiveMedia_DIR "${HIPE_EXTERNAL_DIR}/livemedia" CACHE PATH "LiveMedia_DIR")
  list(APPEND CMAKE_PREFIX_PATH "${LiveMedia_DIR}")

  set(FFmpeg_DIR "${HIPE_EXTERNAL_DIR}/ffmpeg" CACHE PATH "FFMPEG_LIBRARYDIR")
  list(APPEND CMAKE_PREFIX_PATH "${FFmpeg_DIR}")
 
  set(Dlib_DIR "${HIPE_EXTERNAL_DIR}/dlib" CACHE PATH "DLIB_LIBRARYDIR")
  list(APPEND CMAKE_PREFIX_PATH "${Dlib_DIR}")
#   if(HIPE_EXTERNAL_DLIB)
#     list(APPEND CMAKE_PREFIX_PATH "${HIPE_EXTERNAL_DIR}/dlib")
#   endif(HIPE_EXTERNAL_DLIB)
  message(STATUS "CMAKE_PREFIX_PATH: ${CMAKE_PREFIX_PATH}")
endif(WIN32)
