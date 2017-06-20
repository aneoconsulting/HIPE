cmake_minimum_required (VERSION 3.7.1)

SET(HIPE_EXTERNAL "HIPE_EXTERNAL-NOT_FOUND" CACHE PATH "USER VARIABLE for external lib")
if (EXISTS "$ENV{HIPE_EXTERNAL}") 
	 
	SET(HIPE_EXTERNAL "$ENV{HIPE_EXTERNAL}")
else (EXISTS "$ENV{HIPE_EXTERNAL}")
	 if("${HIPE_EXTERNAL}" MATCHES "HIPE_EXTERNAL-NOT_FOUND") 
		message(FATAL_ERROR "Could not find HIPE_EXTERNAL variable. Please set it locally or by environement variable") 
	endif()
endif(EXISTS "$ENV{HIPE_EXTERNAL}")

SET(Hipe_external "${HIPE_EXTERNAL}" CACHE PATH "ROOT of external")


if (WIN32)
SET(HIPE_PLATFORM "win64")
else()
SET(HIPE_PLATFORM "linux64")
endif()

if (WIN32)
SET(Boost_DIR "${Hipe_external}/${HIPE_PLATFORM}/boost_1_62_0/"   CACHE PATH "Boost_DIR" FORCE )
SET(Boost_INCLUDE_DIR "${Boost_DIR}"  CACHE PATH "Boost_INCLUDE_DIR" FORCE )
SET(BOOST_LIBRARYDIR "${Boost_DIR}/lib64-msvc-14.0"  CACHE PATH "BOOST_LIBRARYDIR" FORCE )

SET(OpenCV_DIR "${Hipe_external}/${HIPE_PLATFORM}/opencv/debug"  CACHE PATH "OpenCV Directory" FORCE )

SET(x264_DIR "${Hipe_external}/${HIPE_PLATFORM}/x264-devel-${HIPE_PLATFORM}/" CACHE PATH "x264" FORCE )

SET(LIVE_DIR "${Hipe_external}/${HIPE_PLATFORM}/live555/install"  CACHE PATH "Live_DIR" FORCE )

SET(FFMPEG_DIR "${Hipe_external}/${HIPE_PLATFORM}/ffmpeg"  CACHE PATH "FFMPEG_LIBRARYDIR" FORCE )

SET(SOURCEY_DIR "${Hipe_external}/${HIPE_PLATFORM}/libsourcey"  CACHE PATH "SOURCEY_LIBRARYDIR" FORCE )

SET(WEBRTC_DIR "${Hipe_external}/${HIPE_PLATFORM}/webrtc"  CACHE PATH "SOURCEY_LIBRARYDIR" FORCE )

SET(UV_DIR "${Hipe_external}/${HIPE_PLATFORM}/libuv"  CACHE PATH "LIBUV_LIBRARYDIR" FORCE )

SET(USE_DLIB On  CACHE bool "Activate Dlib library" FORCE )

else()


SET(Boost_DIR "${Hipe_external}/${HIPE_PLATFORM}"   CACHE PATH "Boost_DIR" FORCE )
SET(Boost_INCLUDE_DIR "${Boost_DIR}/include"  CACHE PATH "Boost_INCLUDE_DIR" FORCE )
SET(BOOST_LIBRARYDIR "${Boost_DIR}/lib/"  CACHE PATH "BOOST_LIBRARYDIR" FORCE )

SET(OpenCV_DIR "${Hipe_external}/${HIPE_PLATFORM}/share/OpenCV")

SET(x264_DIR "${Hipe_external}/${HIPE_PLATFORM}/" CACHE PATH "x264" FORCE )

SET(LIVE_DIR "${Hipe_external}/${HIPE_PLATFORM}/"  CACHE PATH "Live_DIR" FORCE )

SET(FFMPEG_DIR "${Hipe_external}/${HIPE_PLATFORM}/"  CACHE PATH "FFMPEG_LIBRARYDIR" FORCE )

SET(DLIB_DIR "${Hipe_external}/${HIPE_PLATFORM}/usr/local/"  CACHE PATH "DLIB_LIBRARYDIR" FORCE )
SET(USE_DLIB On  CACHE bool "Activate Dlib library" FORCE )
endif()
