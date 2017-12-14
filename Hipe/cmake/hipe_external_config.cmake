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

# Make sure HIPE_EXTERNAL exists.
if(NOT EXISTS "${HIPE_EXTERNAL}")
  message(FATAL_ERROR "${HIPE_EXTERNAL} does not exist")
elseif(NOT IS_DIRECTORY "${HIPE_EXTERNAL}")
  message(FATAL_ERROR "${HIPE_EXTERNAL} is not a directory")
endif(NOT EXISTS "${HIPE_EXTERNAL}")



if(WIN32)
  set(HIPE_EXTERNAL_DIR "${HIPE_EXTERNAL}/win64")
else(WIN32)
  set(HIPE_EXTERNAL_DIR "${HIPE_EXTERNAL}/linux64")
endif(WIN32)

if(WIN32)
  set(Boost_DIR "${HIPE_EXTERNAL_DIR}/boost_1_62_0/" CACHE PATH "Boost_DIR" FORCE)
  set(Boost_INCLUDE_DIR "${Boost_DIR}" CACHE PATH "Boost_INCLUDE_DIR" FORCE)
  set(BOOST_LIBRARYDIR "${Boost_DIR}/lib64-msvc-14.0" CACHE PATH "BOOST_LIBRARYDIR" FORCE)
  set(BOOST_ROOT "${Boost_DIR}/"  CACHE PATH "BOOST_ROOT" FORCE)

  set(OpenCV_DIR "${HIPE_EXTERNAL_DIR}/opencv/" CACHE PATH "OpenCV Directory" FORCE)

  set(x264_DIR "${HIPE_EXTERNAL_DIR}/x264-devel-${HIPE_PLATFORM}/" CACHE PATH "x264" FORCE)

  set(LiveMedia_DIR "${HIPE_EXTERNAL_DIR}/live555/install" CACHE PATH "LiveMedia_DIR" FORCE)

  set(FFMPEG_DIR "${HIPE_EXTERNAL_DIR}/ffmpeg" CACHE PATH "FFMPEG_LIBRARYDIR" FORCE)

  set(SOURCEY_DIR "${HIPE_EXTERNAL_DIR}/libsourcey" CACHE PATH "SOURCEY_LIBRARYDIR" FORCE)

  set(WEBRTC_DIR "${HIPE_EXTERNAL_DIR}/webrtc" CACHE PATH "SOURCEY_LIBRARYDIR" FORCE)

  set(UV_DIR "${HIPE_EXTERNAL_DIR}/libuv"  CACHE PATH "LIBUV_LIBRARYDIR" FORCE)


  set(DLIB_DIR "${HIPE_EXTERNAL_DIR}/dlib/"  CACHE PATH "DLIB_LIBRARYDIR" FORCE)

  set(PYTHON27_DIR "${HIPE_EXTERNAL_DIR}/python27/"  CACHE PATH "PYTHON_LIBRARYDIR" FORCE )

  set(YOLOV2_DIR "${HIPE_EXTERNAL_DIR}/yolov2/"  CACHE PATH "YOLOV2_LIBRARYDIR" FORCE )

  set(USE_DLIB On  CACHE bool "Activate Dlib library" FORCE)

  set(OpenBLAS_DIR "${${HIPE_EXTERNAL_DIR}}/OpenBLAS/")

else()
  set(Boost_DIR "${HIPE_EXTERNAL_DIR}" CACHE PATH "Boost_DIR")
  set(Boost_INCLUDE_DIR "${Boost_DIR}/include" CACHE PATH "Boost_INCLUDE_DIR" FORCE)
  set(BOOST_LIBRARYDIR "${Boost_DIR}/lib/" CACHE PATH "BOOST_LIBRARYDIR" FORCE)

  set(OpenCV_DIR "${HIPE_EXTERNAL_DIR}/share/OpenCV" CACHE PATH "OpenCV")

  set(x264_DIR "${HIPE_EXTERNAL_DIR}/" CACHE PATH "x264")

  set(LiveMedia_DIR "${HIPE_EXTERNAL_DIR}/" CACHE PATH "LiveMedia_DIR")

  set(FFMPEG_DIR "${HIPE_EXTERNAL_DIR}/" CACHE PATH "FFMPEG_LIBRARYDIR")

  set(DLIB_DIR "${HIPE_EXTERNAL_DIR}/" CACHE PATH "DLIB_LIBRARYDIR")
  set(PYTHON27_DIR "${HIPE_EXTERNAL_DIR}/python27/"  CACHE PATH "PYTHON_LIBRARYDIR")
endif()
