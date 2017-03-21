cmake_minimum_required (VERSION 3.7.1)

set(COMPONENTS x264 )

macro(_x264_ADJUST_LIB_VARS basename)
	
	if(CMAKE_CONFIGURATION_TYPES OR CMAKE_BUILD_TYPE)
        set(x264_${basename}_LIBRARY optimized ${x264_${basename}_LIBRARY_RELEASE} debug ${x264_${basename}_LIBRARY_DEBUG})
    else()
        # if there are no configuration types and CMAKE_BUILD_TYPE has no value
        # then just use the release libraries
        set(x264_${basename}_LIBRARY ${x264_${basename}_LIBRARY_DEBUG} )
    endif()
      # FIXME: This probably should be set for both cases
    set(x264_${basename}_LIBRARIES optimized ${x264_${basename}_LIBRARY_RELEASE} debug ${x264_${basename}_LIBRARY_DEBUG})
	
endmacro()

set(x264_DIR "x264-ROOT-NOTFOUND" CACHE PATH "Path to the root x264 directory" )
set(x264_LIBRARYDIR "x264-LIBRARY-NOTFOUND" CACHE PATH "Path to the static x264 Library" )


if (${x264_DIR} EQUAL "x264-ROOT-NOTFOUND")
	message( FATAL_ERROR "Variable x264_DIR is empty")
endif()

set(x264_INCLUDEDIR "${x264_DIR}/include" CACHE PATH "include path for x264" FORCE)

if (WIN32)
  set(x264_LIBRARYDIR "${x264_DIR}/lib/x64" CACHE PATH "include path for x264" FORCE)
endif()

if(UNIX)
  set(x264_LIBRARYDIR "${x264_DIR}/lib" CACHE PATH "include path for x264" FORCE)
endif()



set(lib_path ${x264_LIBRARYDIR})
set(_lib_list "")


if (WIN32)
set(EXTENSION .lib)
else()
set(EXTENSION .so)
endif()

foreach( COMPONENT  ${COMPONENTS} )
	string( TOUPPER ${COMPONENT} UPPERCOMPONENT )
	if (NOT TARGET x264::${COMPONENT})
		string( TOUPPER ${COMPONENT} UPPERCOMPONENT )
		set(x264_${UPPERCOMPONENT}_LIBRARY "")
		set(x264_${UPPERCOMPONENT}_LIBRARY_RELEASE "")
		set(x264_${UPPERCOMPONENT}_LIBRARY_DEBUG   "")
	endif()
endforeach()

foreach( COMPONENT  ${COMPONENTS} )
	if (NOT TARGET x264::${COMPONENT})
	string( TOUPPER ${COMPONENT} UPPERCOMPONENT )
		message(STATUS "Set LIBRARY : ${COMPONENT}")
		set(x264_${UPPERCOMPONENT}_LIBRARY "")
		if (WIN32)
		  set(x264_${UPPERCOMPONENT}_LIBRARY_RELEASE "${lib_path}/lib${COMPONENT}${EXTENSION}")
		  set(x264_${UPPERCOMPONENT}_LIBRARY_DEBUG   "${lib_path}/lib${COMPONENT}d${EXTENSION}")
		endif()

		if(UNIX)
		  set(x264_${UPPERCOMPONENT}_LIBRARY_RELEASE "${lib_path}/lib${COMPONENT}${EXTENSION}")
		  set(x264_${UPPERCOMPONENT}_LIBRARY_DEBUG   "${lib_path}/lib${COMPONENT}${EXTENSION}")
		endif()
		
		_x264_ADJUST_LIB_VARS(${UPPERCOMPONENT})
		add_library(x264::${COMPONENT} SHARED IMPORTED)
		message(STATUS "Select LIBRARY : ${x264_${UPPERCOMPONENT}_LIBRARY}")
		
		if(EXISTS "${x264_${UPPERCOMPONENT}_LIBRARY}")
			  set_target_properties(x264::${COMPONENT} PROPERTIES
				IMPORTED_LINK_INTERFACE_LANGUAGES "CXX"
				IMPORTED_LOCATION "${x264_${UPPERCOMPONENT}_LIBRARY}")
		endif()
		if(EXISTS "${x264_${UPPERCOMPONENT}_LIBRARY_RELEASE}")
		  set_property(TARGET x264::${COMPONENT} APPEND PROPERTY
			IMPORTED_CONFIGURATIONS RELEASE)
		  set_target_properties(x264::${COMPONENT} PROPERTIES
			IMPORTED_LINK_INTERFACE_LANGUAGES_RELEASE "CXX"
			IMPORTED_LOCATION_RELEASE "${x264_${UPPERCOMPONENT}_LIBRARY_RELEASE}")
		endif()
		if(EXISTS "${x264_${UPPERCOMPONENT}_LIBRARY_DEBUG}")
		  set_property(TARGET x264::${COMPONENT} APPEND PROPERTY
			IMPORTED_CONFIGURATIONS DEBUG)
		  set_target_properties(x264::${COMPONENT} PROPERTIES
			IMPORTED_LINK_INTERFACE_LANGUAGES_DEBUG "CXX"
			IMPORTED_LOCATION_DEBUG "${x264_${UPPERCOMPONENT}_LIBRARY_DEBUG}")
		endif()
	endif()
endforeach()

set(x264_LIBRARIES ""  CACHE LIST "x264Media libraries " FORCE)

 foreach( COMPONENT  ${COMPONENTS} )
    string( TOUPPER ${COMPONENT} UPPERCOMPONENT )
   
     list(APPEND x264_LIBRARIES ${x264_${UPPERCOMPONENT}_LIBRARY})
    
  endforeach()
 message(STATUS "x264 LIBRARIES : ${x264_LIBRARIES}")
