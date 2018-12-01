if(UNIX)
# message(STATUS "Python36_DIR: ${Python36_DIR}")
# list(APPEND CMAKE_PREFIX_PATH "${Python36_DIR}")
# message(STATUS "PYTHON_LIBRARY: ${PYTHON_LIBRARY}")
# message(STATUS "PYTHON_INCLUDE_DIR: ${PYTHON_INCLUDE_DIR}")

if(CMAKE_SYSTEM_PROCESSOR MATCHES "^(powerpc|ppc)64le")
    set(PYTHONLIBS_FOUND TRUE)
	set(PYTHON36_INCLUDE_DIRS "${Python36_DIR}/include/python3.6")
	SET(PYTHON36LIBS_VERSION_STRING "${PYTHONLIBS_VERSION_STRING}")
    set(PYTHON36_LIBRARIES "${Python36_DIR}/lib/libpython3.6.so")
	SET(PYTHON36_SITE_PACKAGES_DIR "${Python36_DIR}/lib/python3.6/site-packages")
	display_pathlist("PYTHON36_INCLUDE_DIRS" "${PYTHON36_INCLUDE_DIRS}")
	display_pathlist("PYTHON36_LIBRARIES" "${PYTHON36_LIBRARIES}")
	display_pathlist("PYTHON36_SITE_PACKAGES_DIR" "${PYTHON36_SITE_PACKAGES_DIR}")

else()
	find_package(PythonLibs 3.5 REQUIRED)
    set(PYTHONLIBS_FOUND "${PYTHONLIBS_FOUND}")
	set(PYTHON36_INCLUDE_DIRS "${PYTHON_INCLUDE_DIRS}")
	set(PYTHON36LIBS_VERSION_STRING "${PYTHONLIBS_VERSION_STRING}")
    set(PYTHON36_LIBRARIES "${PYTHON_LIBRARIES}")
endif()


# set(Python_ADDITIONAL_VERSIONS 3.7)
if(NOT HIPE_EXTERNAL_PYTHON36)
	if(CMAKE_SYSTEM_PROCESSOR MATCHES "^(powerpc|ppc)64le")
      set(PYTHON_EXECUTABLE "${HIPE_EXTERNAL_DIR}/python3.6.7/bin/python3.6")
	else()
	     find_package(PythonInterp 3.5 REQUIRED)
	endif()
else()
  set(PYTHON_EXECUTABLE "${HIPE_EXTERNAL_DIR}/python36/usr/bin/python36")
endif()
message(STATUS "PYTHON_EXECUTABLE ${PYTHON_EXECUTABLE}")

# Add site-packages to include directories to find Numpy headers.
execute_process(
  COMMAND "${PYTHON_EXECUTABLE}" -c "import site; print(site.getsitepackages()[0])"
  OUTPUT_VARIABLE PYTHON36_SITE_PACKAGES_DIR
  OUTPUT_STRIP_TRAILING_WHITESPACE
  )

# TODO
# Maybe generalize this with file globbing or find_path. Determine how the
# system normally find these files first and use that approach if possible.
list(APPEND PYTHON36_INCLUDE_DIRS "${PYTHON36_SITE_PACKAGES_DIR}/numpy/core/include")


# message(STATUS "PYTHONLIBS_FOUND: ${PYTHONLIBS_FOUND}")
# message(STATUS "PYTHON36LIBS_VERSION_STRING: ${PYTHON36LIBS_VERSION_STRING}")
display_pathlist("PYTHON36_INCLUDE_DIRS" "${PYTHON36_INCLUDE_DIRS}")
display_pathlist("PYTHON36_LIBRARIES" "${PYTHON36_LIBRARIES}")
display_pathlist("PYTHON36_SITE_PACKAGES_DIR" "${PYTHON36_SITE_PACKAGES_DIR}")


else(UNIX)

cmake_minimum_required (VERSION 3.7.1)

macro(_Python36_ADJUST_LIB_VARS basename)

	if(CMAKE_CONFIGURATION_TYPES OR CMAKE_BUILD_TYPE)
        set(Python36_${basename}_LIBRARY optimized ${Python36_${basename}_LIBRARY_RELEASE} debug ${Python36_${basename}_LIBRARY_DEBUG})
    else()
        # if there are no configuration types and CMAKE_BUILD_TYPE has no value
        # then just use the release libraries
        set(Python36_${basename}_LIBRARY ${Python36_${basename}_LIBRARY_DEBUG} )
    endif()
      # FIXME: This probably should be set for both cases
    set(Python36_${basename}_LIBRARIES optimized ${Python36_${basename}_LIBRARY_RELEASE} debug ${Python36_${basename}_LIBRARY_DEBUG})

endmacro()

set(Python36_DIR "PYTHON36-ROOT-NOTFOUND" CACHE PATH "Path to the root python36 directory" )
#set(PYTHON36_LIBRARYDIR "PYTHON36-LIBRARY-NOTFOUND" CACHE PATH "Path to the static python36 Library" )


if (${Python36_DIR} EQUAL "PYTHON36-ROOT-NOTFOUND")
	message( FATAL_ERROR "Variable Python36_DIR is empty")
endif()

if (WIN32)

set(PYTHON36_INCLUDE_DIRS "${Python36_DIR}/include;${Python36_DIR}/lib/site-packages/numpy/core/include" CACHE PATH "include path for python36" FORCE)

set(Python36_LIBRARY_DIR "${Python36_DIR}/libs" CACHE PATH "include path for python36" FORCE)
else()
set(PYTHON36_INCLUDE_DIRS "/usr/include/python3.6/;/usr/lib/python3.6/dist-packages/numpy/core/include" CACHE PATH "include path for python36" FORCE)

set(Python36_LIBRARY_DIR "/usr/lib/x86_64-linux-gnu/" CACHE PATH "include path for python36" FORCE)

endif()

if (WIN32)
set(COMPONENTS python36 )
else()
  set(COMPONENTS python3.6 )
endif()
set(lib_path ${Python36_LIBRARY_DIR})
set(_lib_list "")


if (WIN32)
set(EXTENSION .lib)
else()
set(EXTENSION .so)
endif()

foreach( COMPONENT  ${COMPONENTS} )
	string( TOUPPER ${COMPONENT} UPPERCOMPONENT )
	if (NOT TARGET python36::${COMPONENT})
		string( TOUPPER ${COMPONENT} UPPERCOMPONENT )
		set(Python36_${UPPERCOMPONENT}_LIBRARY "")
		set(Python36_${UPPERCOMPONENT}_LIBRARY_RELEASE "")
		set(Python36_${UPPERCOMPONENT}_LIBRARY_DEBUG   "")
	endif()
endforeach()

foreach( COMPONENT  ${COMPONENTS} )
	if (NOT TARGET python36::${COMPONENT})
	string( TOUPPER ${COMPONENT} UPPERCOMPONENT )
		message(STATUS "Set LIBRARY : ${COMPONENT}")
		set(Python36_${UPPERCOMPONENT}_LIBRARY "")

		if(WIN32)
			set(Python36_${UPPERCOMPONENT}_LIBRARY_RELEASE "${lib_path}/${COMPONENT}${EXTENSION}")
			set(Python36_${UPPERCOMPONENT}_LIBRARY_DEBUG   "${lib_path}/${COMPONENT}_d${EXTENSION};${lib_path}/${COMPONENT}${EXTENSION}")
			set(COMMON_PYTHON36_LIB_DEPS "ws2_32;winmm;comctl32;gdi32;imm32")
			list(APPEND Python36_${UPPERCOMPONENT}_LIBRARY_RELEASE "${COMMON_PYTHON36_LIB_DEPS}")
			list(APPEND Python36_${UPPERCOMPONENT}_LIBRARY_DEBUG "${COMMON_PYTHON36_LIB_DEPS}")
		endif()

	  if (UNIX)
		set(Python36_${UPPERCOMPONENT}_LIBRARY_RELEASE "${lib_path}/lib${COMPONENT}${EXTENSION}")
		set(Python36_${UPPERCOMPONENT}_LIBRARY_DEBUG   "${lib_path}/lib${COMPONENT}${EXTENSION}")
	  endif()

		_Python36_ADJUST_LIB_VARS(${UPPERCOMPONENT})
		add_library(python36::${COMPONENT} STATIC IMPORTED)
		message(STATUS "Select LIBRARY : ${Python36_${UPPERCOMPONENT}_LIBRARY}")

		if(EXISTS "${Python36_${UPPERCOMPONENT}_LIBRARY}")
			  set_target_properties(python36::${COMPONENT} PROPERTIES
				IMPORTED_LINK_INTERFACE_LANGUAGES "CXX"
				IMPORTED_LOCATION "${Python36_${UPPERCOMPONENT}_LIBRARY}")
		endif()
		if(EXISTS "${Python36_${UPPERCOMPONENT}_LIBRARY_RELEASE}")
		  set_property(TARGET python36::${COMPONENT} APPEND PROPERTY
			IMPORTED_CONFIGURATIONS RELEASE)
		  set_target_properties(python36::${COMPONENT} PROPERTIES
			IMPORTED_LINK_INTERFACE_LANGUAGES_RELEASE "CXX"
			IMPORTED_LOCATION_RELEASE "${Python36_${UPPERCOMPONENT}_LIBRARY_RELEASE}")
		endif()
		if(EXISTS "${Python36_${UPPERCOMPONENT}_LIBRARY_DEBUG}")
		  set_property(TARGET python36::${COMPONENT} APPEND PROPERTY
			IMPORTED_CONFIGURATIONS DEBUG)
		  set_target_properties(python36::${COMPONENT} PROPERTIES
			IMPORTED_LINK_INTERFACE_LANGUAGES_DEBUG "CXX"
			IMPORTED_LOCATION_DEBUG "${Python36_${UPPERCOMPONENT}_LIBRARY_DEBUG}")
		endif()
	endif()
endforeach()

set(PYTHON36_LIBRARIES ""  CACHE LIST "Python36Media libraries " FORCE)

 foreach( COMPONENT  ${COMPONENTS} )
    string( TOUPPER ${COMPONENT} UPPERCOMPONENT )

     list(APPEND PYTHON36_LIBRARIES ${Python36_${UPPERCOMPONENT}_LIBRARY})

  endforeach()


message(STATUS "Python36 LIBRARIES : ${PYTHON36_LIBRARIES}")

endif(UNIX)

prepend_include_directories_if_necessary("${PYTHON36_INCLUDE_DIRS}")
