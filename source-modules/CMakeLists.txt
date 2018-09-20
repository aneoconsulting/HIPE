cmake_minimum_required (VERSION 3.7.1)



project (Hipe_module)

option(INSTALL_SOURCE "Install source for future developpement" OFF)
message(STATUS "INSTALL_SOURCE: ${INSTALL_SOURCE}")

# Options
option(USE_GPERFTOOLS "Use gperftools." OFF)
option(USE_DLIB "Enable dlib library." ON)
# option(HIPE_STATIC_LIBS "Compile static libraries." ON)
option(HIPE_STATIC_EXES "Compile static executables." OFF)

if (WIN32)
  option(HIPE_EXTERNAL_PYTHON27 "Use Python 2.7 libraries in Hipe External." ON)
else()
  option(HIPE_EXTERNAL_PYTHON27 "Use Python 2.7 libraries in Hipe External." OFF)
 endif()

option(HIPE_EXTERNAL_BOOST "Use Boost libraries in Hipe External." ON)

option(HIPE_EXTERNAL_OPENCV "Use OpenCV libraries in Hipe External." ON)

option(HIPE_EXTERNAL_GLOG "Use GLOB libraries in Hipe External." ON)

option(WITH_CUDA "Use CUDA GPU." ON)

option(WITH_CUBLAS "Use CUBLAS libraries." OFF)

option(WITH_CAFFE "Use Caffe libraries." OFF)

option(WITH_OPENBLAS "Use OpenBLAS libraries." OFF)


option(WITH_LEVELDB "Use Caffe libraries." OFF)

option(WITH_LMDB "Use LMDB libraries." OFF)

option(WITH_HDF5 "Use HDF5 libraries." OFF)

option(WITH_GFLAGS "Use GFlags libraries." ON)

option(WITH_GLOG "Use GLOG libraries." ON)

option(WITH_SNAPPY "Use SNAPPY libraries." OFF)


option(WITH_PROTOBUF "Use PROTOBUF libraries." OFF)

option(WITH_WEBRTC "Use WebRTC libraries." ON)

set(WITH_WEBRTC ON CACHE BOOL "Use WebRTC libraries." FORCE)

# option(HIPE_EXTERNAL_DLIB "Use Dlib libraries in Hipe External." ON)
# message(STATUS "HIPE_EXTERNAL_DLIB: ${HIPE_EXTERNAL_DLIB}")

if (WIN32)
option(CMAKE_USE_WIN32_THREADS_INIT "using WIN32 threads" ON)
endif(WIN32)

if (WITH_CAFFE AND UNIX)
#Disable Caffe TODO ATM IT'S NOT IMPLEMENTED
set(WITH_CAFFE OFF CACHE BOOL "Use Caffe libraries." FORCE)
message(ERROR "Cannot yet activate the Caffe on Linux. Implementation soon")

elseif(WITH_CAFFE AND WIN32)

set(WITH_LMDB ON CACHE BOOL "Use Caffe libraries." FORCE)
set(WITH_OPENBLAS ON CACHE BOOL "Use OpenBLAS libraries." FORCE)
set(WITH_LEVELDB ON CACHE BOOL "Use Caffe libraries." FORCE)
set(WITH_LMDB ON CACHE BOOL "Use LMDB libraries." FORCE)
set(WITH_HDF5 ON CACHE BOOL "Use HDF5 libraries." FORCE)
set(WITH_GFLAGS ON CACHE BOOL "Use GFlags libraries." FORCE)
set(WITH_GLOG ON CACHE BOOL "Use GLOG libraries." FORCE)
set(WITH_SNAPPY ON CACHE BOOL "Use SNAPPY libraries." FORCE)
set(WITH_CUDA ON CACHE BOOL "Use CUDA GPU."  FORCE)
set(WITH_CUBLAS ON CACHE BOOL "Use CUBLAS libraries." FORCE)
set(WITH_PROTOBUF ON CACHE BOOL "Use PROTOBUF libraries." FORCE)


endif()



message(STATUS "HIPE_EXTERNAL_PYTHON27: ${HIPE_EXTERNAL_PYTHON27}")
message(STATUS "HIPE_EXTERNAL_BOOST: ${HIPE_EXTERNAL_BOOST}")
message(STATUS "HIPE_EXTERNAL_OPENCV: ${HIPE_EXTERNAL_OPENCV}")
message(STATUS "WITH_CUDA: ${WITH_CUDA}")
message(STATUS "WITH_CUBLAS: ${WITH_CUBLAS}")
message(STATUS "WITH_CAFFE: ${WITH_CAFFE}")
message(STATUS "WITH_OPENBLAS: ${WITH_OPENBLAS}")
message(STATUS "WITH_LEVELDB: ${WITH_LEVELDB}")
message(STATUS "WITH_LMDB: ${WITH_LMDB}")
message(STATUS "WITH_HDF5: ${WITH_HDF5}")
message(STATUS "WITH_GFLAGS: ${WITH_GFLAGS}")
message(STATUS "WITH_GLOG: ${WITH_GLOG}")
message(STATUS "WITH_SNAPPY: ${WITH_SNAPPY}")
message(STATUS "WITH_PROTOBUF: ${WITH_PROTOBUF}")


# Configure common settings.
include("${CMAKE_SOURCE_DIR}/cmake/common_config.cmake")

message(STATUS "Change path to install directory [${CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT}]")
IF(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
  SET(CMAKE_INSTALL_PREFIX "${CMAKE_SOURCE_DIR}/../install/hipe-modules" CACHE PATH "Installation directory" FORCE)
ENDIF(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)

#controls where to build the executables after "make"
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/target)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/target/libs)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/target/libs)

#Add some multithreaded build support =====================================================================================================
if(WIN32)
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MP")

endif()

add_subdirectory(source)

set_property (DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT filter)
# add_subdirectory(tests)


install(CODE "set(PROJECT_SOURCE_DIR \"\${CMAKE_INSTALL_PREFIX}/project\")" COMPONENT sources)
install(CODE "set(LICENSE_DIR \"${PROJECT_SOURCE_DIR}\")" COMPONENT sources)
install(CODE "set(HIPE_INSTALL_DIR \"${CMAKE_INSTALL_PREFIX}\")" COMPONENT sources)

if (${INSTALL_SOURCE})
	install(DIRECTORY "${CMAKE_SOURCE_DIR}/" DESTINATION "./project"
			COMPONENT sources
			PATTERN "cmake" 
			PATTERN "source"
			PATTERN "header"
			PATTERN "*.txt"
			PATTERN "*.cmake"
			PATTERN "*.bat"
			PATTERN "*.sh"
			PATTERN "bin" EXCLUDE
			PATTERN "build" EXCLUDE
			PATTERN "install" EXCLUDE
			)

	install (SCRIPT "${CMAKE_SOURCE_DIR}/cmake/postInstall.cmake" COMPONENT sources)
endif()


set(CPACK_PACKAGE_NAME "Hipe-Plugins-imageFilterMod")
set(CPACK_PACKAGE_VENDOR "Damien DUBUC (ANEO)")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Hipe Image processing Filter module to use with Hipe core service")

#TODO Get the version from BUILD-VERSION.txt
set(CPACK_PACKAGE_VERSION_MAJOR "2")
set(CPACK_PACKAGE_VERSION_MINOR "1")
set(CPACK_PACKAGE_VERSION_PATCH "0")
set(CPACK_PACKAGE_VERSION "${CPACK_PACKAGE_VERSION_MAJOR}.${CPACK_PACKAGE_VERSION_MINOR}.${CPACK_PACKAGE_VERSION_PATCH}")
set(CPACK_PACKAGE_INSTALL_DIRECTORY "ANEO\\\\HIPE\\\\hipe-modules")
set(CPACK_COMPONENTS_ALL library deps runtime)

if (${INSTALL_SOURCE})
	set(CPACK_COMPONENTS_ALL ${CPACK_COMPONENTS_ALL} sources)
endif()

set(CPACK_COMPONENT_CTDATA_GROUP "data")
set(CPACK_COMPONENT_INSTRUCTIONDATA_GROUP "data")

set(CPACK_COMPONENT_APPLICATIONS_DISPLAY_NAME "Hipe-ImageProcessing-mod")
set(CPACK_COMPONENT_GROUP_DATA_DESCRIPTION "data (CT and instructions)")

set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_SOURCE_DIR}/../LICENCE_AGPL.txt")

set(CPACK_PACKAGE_ICON "${CMAKE_SOURCE_DIR}\\\\..\\\\ressources\\\\logo.ico")

if (WIN32)
set(CPACK_NSIS_MUI_ICON "${CMAKE_SOURCE_DIR}/../ressources/logo.ico")
set(CPACK_NSIS_MUI_UNIICON "${CMAKE_SOURCE_DIR}/../ressources/logo.ico")


#set(CPACK_NSIS_MENU_LINKS "programs\\hipe" "Hipe")
set(CPACK_NSIS_CREATE_ICONS_EXTRA
    "CreateShortCut '$SMPROGRAMS\\\\$STARTMENU_FOLDER\\\\modules.lnk' 'C:\\\\windows\\\\explorer.exe' '$INSTDIR'")

endif(WIN32)
			  
include(CPack)
						  
						  
#
#Copy in install folder all dependencies for hipe_core
#

#create directory for 3rdParty


