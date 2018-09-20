cmake_minimum_required (VERSION 3.7.1)



project (Hipe)

if (MSVC)
option(USE_PREBUILT_DEPENDENCIES "Download and use the prebuilt dependencies" ON)
option(FORCE_USE_PREBUILT_DEPENDENCIES "Download and use the prebuilt dependencies" OFF)
endif()

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
message(STATUS "HIPE_EXTERNAL_PYTHON27: ${HIPE_EXTERNAL_PYTHON27}")

option(HIPE_EXTERNAL_BOOST "Use Boost libraries in Hipe External." ON)
message(STATUS "HIPE_EXTERNAL_BOOST: ${HIPE_EXTERNAL_BOOST}")

option(HIPE_EXTERNAL_OPENCV "Use OpenCV libraries in Hipe External." ON)
message(STATUS "HIPE_EXTERNAL_OPENCV: ${HIPE_EXTERNAL_OPENCV}")


option(HIPE_EXTERNAL_GLOG "Use GLOB libraries in Hipe External." ON)
message(STATUS "HIPE_EXTERNAL_GLOG: ${HIPE_EXTERNAL_GLOG}")

option(INSTALL_SOURCE "Install source for future developpement" ON)
message(STATUS "INSTALL_SOURCE: ${INSTALL_SOURCE}")

# Configure common settings.
include("${CMAKE_SOURCE_DIR}/cmake/common_config.cmake")


if (WIN32)

include("${CMAKE_SOURCE_DIR}/cmake/WindowsDownloadPrebuiltDependencies.cmake")

option(CMAKE_USE_WIN32_THREADS_INIT "using WIN32 threads" ON)

set(CMAKE_INSTALL_SYSTEM_RUNTIME_LIBS_SKIP TRUE)
include(InstallRequiredSystemLibraries)

endif(WIN32)

set(WITH_GFLAGS ON CACHE BOOL "Use GFlags libraries." FORCE)
set(WITH_GLOG ON CACHE BOOL "Use GLOG libraries." FORCE)

#controls where to build the executables after "make"
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/target)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/target/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/target/libs)
set(CMAKE_INCLUDE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/target/include)

message(STATUS "Change path to install directory [${CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT}]")
IF(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
  SET(CMAKE_INSTALL_PREFIX "${CMAKE_SOURCE_DIR}/../install/hipe-core" CACHE PATH "Installation directory" FORCE)
ENDIF(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)

#Add some multithreaded build support =====================================================================================================
if(WIN32)
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MP")
endif()

add_subdirectory(source)




install(CODE "set(PROJECT_SOURCE_DIR \"\${CMAKE_INSTALL_PREFIX}/project\")" COMPONENT sources)
install(CODE "set(LICENSE_DIR \"${PROJECT_SOURCE_DIR}\")" COMPONENT sources)
install(CODE "set(HIPE_INSTALL_DIR \"${CMAKE_INSTALL_PREFIX}\")" COMPONENT sources)

if (${INSTALL_SOURCE})

	install(DIRECTORY "${CMAKE_SOURCE_DIR}/" DESTINATION "./project"
			COMPONENT sources
			PATTERN "cmake"
			PATTERN "Documentation"
			PATTERN "header"
			PATTERN "source"
			PATTERN "scripts"
			PATTERN "ressources"
			PATTERN "tests"
			PATTERN "*.bat"
			PATTERN "*.txt"
			PATTERN "*.md"
			PATTERN "build" EXCLUDE
			PATTERN "bin" EXCLUDE
			)

	install (SCRIPT "${CMAKE_SOURCE_DIR}/cmake/postInstall.cmake" COMPONENT sources)
endif()

##Install working directory
install(DIRECTORY "${CMAKE_SOURCE_DIR}/../workingdir" DESTINATION "./" COMPONENT userspace)

set(CPACK_PACKAGE_NAME "Hipe-core-installer")
set(CPACK_PACKAGE_VENDOR "Damien DUBUC (ANEO)")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Hipe Core is the platform to load modules filter")

#TODO Get the version from BUILD-VERSION.txt
set(CPACK_PACKAGE_VERSION_MAJOR "2")
set(CPACK_PACKAGE_VERSION_MINOR "1")
set(CPACK_PACKAGE_VERSION_PATCH "0")
set(CPACK_PACKAGE_VERSION "${CPACK_PACKAGE_VERSION_MAJOR}.${CPACK_PACKAGE_VERSION_MINOR}.${CPACK_PACKAGE_VERSION_PATCH}")
set(CPACK_PACKAGE_INSTALL_DIRECTORY "ANEO\\\\HIPE\\\\hipe-core")
set(CPACK_COMPONENTS_ALL applications library headers)

if (${INSTALL_SOURCE})
	set(CPACK_COMPONENTS_ALL ${CPACK_COMPONENTS_ALL} sources)
	set(CPACK_COMPONENTS_ALL ${CPACK_COMPONENTS_ALL} sources)
endif()

set(CPACK_COMPONENTS_ALL ${CPACK_COMPONENTS_ALL} userspace)

set(CPACK_COMPONENTS_GROUPING ALL_COMPONENTS_IN_ONE)
set(CPACK_COMPONENT_APPLICATIONS_GROUP "")


set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_SOURCE_DIR}/../LICENCE_AGPL.txt")

set(CPACK_PACKAGE_ICON "${CMAKE_SOURCE_DIR}\\\\..\\\\ressources\\\\logo.ico")

if (WIN32)
	set(CPACK_NSIS_MUI_ICON "${CMAKE_SOURCE_DIR}/../ressources/logo.ico")
	set(CPACK_NSIS_MUI_UNIICON "${CMAKE_SOURCE_DIR}/../ressources/logo.ico")

	set(CPACK_PACKAGE_EXECUTABLES "ANEO\\\\HIPE\\\\hipe-core\\\\bin\\\\Release\\\\starthipe.bat" "Start service HIPE")

	set(CPACK_NSIS_MENU_LINKS "bin/Release/starthipe.bat" "Start service HIPE")
							  #${CT_DATA_FILE_DIR} "CT Data"
							  #"data\\example.pdf" "documentation")
	set(CPACK_NSIS_CREATE_ICONS_EXTRA
		"CreateShortCut '$SMPROGRAMS\\\\$STARTMENU_FOLDER\\\\hipe-core.lnk' 'C:\\\\windows\\\\explorer.exe' '$INSTDIR'")
endif(WIN32)
			  
include(CPack)
						  
						  
