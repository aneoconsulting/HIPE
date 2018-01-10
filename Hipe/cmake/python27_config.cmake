cmake_minimum_required (VERSION 3.7.1)

cmake_minimum_required (VERSION 3.7.1)


macro(_Python27_ADJUST_LIB_VARS basename)
	
	if(CMAKE_CONFIGURATION_TYPES OR CMAKE_BUILD_TYPE)
        set(Python27_${basename}_LIBRARY optimized ${Python27_${basename}_LIBRARY_RELEASE} debug ${Python27_${basename}_LIBRARY_DEBUG})
    else()
        # if there are no configuration types and CMAKE_BUILD_TYPE has no value
        # then just use the release libraries
        set(Python27_${basename}_LIBRARY ${Python27_${basename}_LIBRARY_DEBUG} )
    endif()
      # FIXME: This probably should be set for both cases
    set(Python27_${basename}_LIBRARIES optimized ${Python27_${basename}_LIBRARY_RELEASE} debug ${Python27_${basename}_LIBRARY_DEBUG})
	
endmacro()

set(PYTHON27_DIR "PYTHON27-ROOT-NOTFOUND" CACHE PATH "Path to the root python27 directory" )
#set(PYTHON27_LIBRARYDIR "PYTHON27-LIBRARY-NOTFOUND" CACHE PATH "Path to the static python27 Library" )


if (${PYTHON27_DIR} EQUAL "PYTHON27-ROOT-NOTFOUND")
	message( FATAL_ERROR "Variable PYTHON27_DIR is empty")
endif()

if (WIN32)

set(Python27_INCLUDEDIR "${PYTHON27_DIR}/include;${PYTHON27_DIR}/lib/site-packages/numpy/core/include" CACHE PATH "include path for python27" FORCE)

set(Python27_LIBRARY_DIR "${PYTHON27_DIR}/libs" CACHE PATH "include path for python27" FORCE)
else()
set(Python27_INCLUDEDIR "/usr/include/python2.7/;/usr/lib/python2.7/dist-packages/numpy/core/include" CACHE PATH "include path for python27" FORCE)

set(Python27_LIBRARY_DIR "/usr/lib/x86_64-linux-gnu/" CACHE PATH "include path for python27" FORCE)

endif()

if (WIN32)
set(COMPONENTS python27 )
else()
  set(COMPONENTS python2.7 )
endif()
set(lib_path ${Python27_LIBRARY_DIR})
set(_lib_list "")


if (WIN32)
set(EXTENSION .lib)
else()
set(EXTENSION .so)
endif()

foreach( COMPONENT  ${COMPONENTS} )
	string( TOUPPER ${COMPONENT} UPPERCOMPONENT )
	if (NOT TARGET python27::${COMPONENT})
		string( TOUPPER ${COMPONENT} UPPERCOMPONENT )
		set(Python27_${UPPERCOMPONENT}_LIBRARY "")
		set(Python27_${UPPERCOMPONENT}_LIBRARY_RELEASE "")
		set(Python27_${UPPERCOMPONENT}_LIBRARY_DEBUG   "")
	endif()
endforeach()

foreach( COMPONENT  ${COMPONENTS} )
	if (NOT TARGET python27::${COMPONENT})
	string( TOUPPER ${COMPONENT} UPPERCOMPONENT )
		message(STATUS "Set LIBRARY : ${COMPONENT}")
		set(Python27_${UPPERCOMPONENT}_LIBRARY "")

		if(WIN32)
			set(Python27_${UPPERCOMPONENT}_LIBRARY_RELEASE "${lib_path}/${COMPONENT}${EXTENSION}")
			set(Python27_${UPPERCOMPONENT}_LIBRARY_DEBUG   "${lib_path}/${COMPONENT}_d${EXTENSION};${lib_path}/${COMPONENT}${EXTENSION}")
			set(COMMON_PYTHON27_LIB_DEPS "ws2_32;winmm;comctl32;gdi32;imm32")
			list(APPEND Python27_${UPPERCOMPONENT}_LIBRARY_RELEASE "${COMMON_PYTHON27_LIB_DEPS}")
			list(APPEND Python27_${UPPERCOMPONENT}_LIBRARY_DEBUG "${COMMON_PYTHON27_LIB_DEPS}")
		endif()

	  if (UNIX)
		set(Python27_${UPPERCOMPONENT}_LIBRARY_RELEASE "${lib_path}/lib${COMPONENT}${EXTENSION}")
		set(Python27_${UPPERCOMPONENT}_LIBRARY_DEBUG   "${lib_path}/lib${COMPONENT}${EXTENSION}")
	  endif()
	  
		_Python27_ADJUST_LIB_VARS(${UPPERCOMPONENT})
		add_library(python27::${COMPONENT} STATIC IMPORTED)
		message(STATUS "Select LIBRARY : ${Python27_${UPPERCOMPONENT}_LIBRARY}")
		
		if(EXISTS "${Python27_${UPPERCOMPONENT}_LIBRARY}")
			  set_target_properties(python27::${COMPONENT} PROPERTIES
				IMPORTED_LINK_INTERFACE_LANGUAGES "CXX"
				IMPORTED_LOCATION "${Python27_${UPPERCOMPONENT}_LIBRARY}")
		endif()
		if(EXISTS "${Python27_${UPPERCOMPONENT}_LIBRARY_RELEASE}")
		  set_property(TARGET python27::${COMPONENT} APPEND PROPERTY
			IMPORTED_CONFIGURATIONS RELEASE)
		  set_target_properties(python27::${COMPONENT} PROPERTIES
			IMPORTED_LINK_INTERFACE_LANGUAGES_RELEASE "CXX"
			IMPORTED_LOCATION_RELEASE "${Python27_${UPPERCOMPONENT}_LIBRARY_RELEASE}")
		endif()
		if(EXISTS "${Python27_${UPPERCOMPONENT}_LIBRARY_DEBUG}")
		  set_property(TARGET python27::${COMPONENT} APPEND PROPERTY
			IMPORTED_CONFIGURATIONS DEBUG)
		  set_target_properties(python27::${COMPONENT} PROPERTIES
			IMPORTED_LINK_INTERFACE_LANGUAGES_DEBUG "CXX"
			IMPORTED_LOCATION_DEBUG "${Python27_${UPPERCOMPONENT}_LIBRARY_DEBUG}")
		endif()
	endif()
endforeach()

set(Python27_LIBRARIES ""  CACHE LIST "Python27Media libraries " FORCE)

 foreach( COMPONENT  ${COMPONENTS} )
    string( TOUPPER ${COMPONENT} UPPERCOMPONENT )
   
     list(APPEND Python27_LIBRARIES ${Python27_${UPPERCOMPONENT}_LIBRARY})
    
  endforeach()


message(STATUS "Python27 LIBRARIES : ${Python27_LIBRARIES}")
