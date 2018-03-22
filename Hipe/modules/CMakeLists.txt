cmake_minimum_required (VERSION 3.7.1)

#Add some multithreaded build support =====================================================================================================
if(WIN32)
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MP")

endif()

project (Hipe_module)

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
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/target/libs)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/target/libs)

add_subdirectory(source)
# add_subdirectory(tests)

install(DIRECTORY "${CMAKE_SOURCE_DIR}/" DESTINATION "./"
		COMPONENT sources
        PATTERN "cmake" 
		PATTERN "bin" 
		PATTERN "source"
		PATTERN "header"
		PATTERN "*.txt"
		PATTERN "build" EXCLUDE
		)


set(CPACK_PACKAGE_NAME "Hipe-Plugins-imageFilterMod")
set(CPACK_PACKAGE_VENDOR "Damien DUBUC (ANEO)")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Hipe Image processing Filter module to use with Hipe core service")

#TODO Get the version from BUILD-VERSION.txt
set(CPACK_PACKAGE_VERSION_MAJOR "2")
set(CPACK_PACKAGE_VERSION_MINOR "1")
set(CPACK_PACKAGE_VERSION_PATCH "0")
set(CPACK_PACKAGE_VERSION "${CPACK_PACKAGE_VERSION_MAJOR}.${CPACK_PACKAGE_VERSION_MINOR}.${CPACK_PACKAGE_VERSION_PATCH}")
set(CPACK_PACKAGE_INSTALL_DIRECTORY "Hipe-Modules")
set(CPACK_COMPONENTS_ALL library sources bin deps runtime)
set(CPACK_COMPONENT_CTDATA_GROUP "data")
set(CPACK_COMPONENT_INSTRUCTIONDATA_GROUP "data")

set(CPACK_COMPONENT_APPLICATIONS_DISPLAY_NAME "Hipe-ImageProcessing-mod")
set(CPACK_COMPONENT_GROUP_DATA_DESCRIPTION "data (CT and instructions)")

set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_SOURCE_DIR}/../../LICENSE-MODULES.txt")

set(CPACK_PACKAGE_ICON "${CMAKE_SOURCE_DIR}\\\\..\\\\ressources\\\\logo.ico")

if (WIN32)
set(CPACK_NSIS_MUI_ICON "${CMAKE_SOURCE_DIR}/../ressources/logo.ico")
set(CPACK_NSIS_MUI_UNIICON "${CMAKE_SOURCE_DIR}/../ressources/logo.ico")


set(CPACK_NSIS_MENU_LINKS "programs\\hipe" "Hipe")
                          #${CT_DATA_FILE_DIR} "CT Data"
                          #"data\\example.pdf" "documentation")
set(CPACK_NSIS_MODIFY_PATH ON)
endif(WIN32)
			  
include(CPack)
						  
						  
#
#Copy in install folder all dependencies for hipe_core
#

#create directory for 3rdParty


