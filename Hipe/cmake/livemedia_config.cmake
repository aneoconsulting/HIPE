cmake_minimum_required (VERSION 3.7.1)


macro(_Live_ADJUST_LIB_VARS basename)
	
	if(CMAKE_CONFIGURATION_TYPES OR CMAKE_BUILD_TYPE)
        set(Live_${basename}_LIBRARY optimized ${Live_${basename}_LIBRARY_RELEASE} debug ${Live_${basename}_LIBRARY_DEBUG})
    else()
        # if there are no configuration types and CMAKE_BUILD_TYPE has no value
        # then just use the release libraries
        set(Live_${basename}_LIBRARY ${Live_${basename}_LIBRARY_DEBUG} )
    endif()
      # FIXME: This probably should be set for both cases
    set(Live_${basename}_LIBRARIES optimized ${Live_${basename}_LIBRARY_RELEASE} debug ${Live_${basename}_LIBRARY_DEBUG})
	
endmacro()

set(LiveMedia_DIR "LIVE-ROOT-NOTFOUND" CACHE PATH "Path to the root live directory" )
#set(LIVE_LIBRARYDIR "LIVE-LIBRARY-NOTFOUND" CACHE PATH "Path to the static live Library" )


if (${LiveMedia_DIR} EQUAL "LIVE-ROOT-NOTFOUND")
	message( FATAL_ERROR "Variable LiveMedia_DIR is empty")
endif()

set(Live_INCLUDEDIR "${LiveMedia_DIR}/include" CACHE PATH "include path for live" FORCE)

set(Live_LIBRARDIR "${LiveMedia_DIR}/lib" CACHE PATH "include path for live" FORCE)

set(COMPONENTS liveMedia groupsock  BasicUsageEnvironment UsageEnvironment )

set(lib_path ${Live_LIBRARDIR})
set(_lib_list "")


if (WIN32)
set(EXTENSION .lib)
else()
set(EXTENSION .a)
endif()

foreach( COMPONENT  ${COMPONENTS} )
	string( TOUPPER ${COMPONENT} UPPERCOMPONENT )
	if (NOT TARGET live::${COMPONENT})
		string( TOUPPER ${COMPONENT} UPPERCOMPONENT )
		set(Live_${UPPERCOMPONENT}_LIBRARY "")
		set(Live_${UPPERCOMPONENT}_LIBRARY_RELEASE "")
		set(Live_${UPPERCOMPONENT}_LIBRARY_DEBUG   "")
	endif()
endforeach()

foreach( COMPONENT  ${COMPONENTS} )
	if (NOT TARGET live::${COMPONENT})
	string( TOUPPER ${COMPONENT} UPPERCOMPONENT )
		message(STATUS "Set LIBRARY : ${COMPONENT}")
		set(Live_${UPPERCOMPONENT}_LIBRARY "")

		if(WIN32)
		set(Live_${UPPERCOMPONENT}_LIBRARY_RELEASE "${lib_path}/release/${COMPONENT}${EXTENSION}")
		set(Live_${UPPERCOMPONENT}_LIBRARY_DEBUG   "${lib_path}/debug/${COMPONENT}${EXTENSION}")
	  endif()

	  if (UNIX)
		set(Live_${UPPERCOMPONENT}_LIBRARY_RELEASE "${lib_path}/lib${COMPONENT}${EXTENSION}")
		set(Live_${UPPERCOMPONENT}_LIBRARY_DEBUG   "${lib_path}/lib${COMPONENT}${EXTENSION}")
	  endif()
	  
		_Live_ADJUST_LIB_VARS(${UPPERCOMPONENT})
		add_library(live::${COMPONENT} STATIC IMPORTED)
		message(STATUS "Select LIBRARY : ${Live_${UPPERCOMPONENT}_LIBRARY}")
		
		if(EXISTS "${Live_${UPPERCOMPONENT}_LIBRARY}")
			  set_target_properties(live::${COMPONENT} PROPERTIES
				IMPORTED_LINK_INTERFACE_LANGUAGES "CXX"
				IMPORTED_LOCATION "${Live_${UPPERCOMPONENT}_LIBRARY}")
		endif()
		if(EXISTS "${Live_${UPPERCOMPONENT}_LIBRARY_RELEASE}")
		  set_property(TARGET live::${COMPONENT} APPEND PROPERTY
			IMPORTED_CONFIGURATIONS RELEASE)
		  set_target_properties(live::${COMPONENT} PROPERTIES
			IMPORTED_LINK_INTERFACE_LANGUAGES_RELEASE "CXX"
			IMPORTED_LOCATION_RELEASE "${Live_${UPPERCOMPONENT}_LIBRARY_RELEASE}")
		endif()
		if(EXISTS "${Live_${UPPERCOMPONENT}_LIBRARY_DEBUG}")
		  set_property(TARGET live::${COMPONENT} APPEND PROPERTY
			IMPORTED_CONFIGURATIONS DEBUG)
		  set_target_properties(live::${COMPONENT} PROPERTIES
			IMPORTED_LINK_INTERFACE_LANGUAGES_DEBUG "CXX"
			IMPORTED_LOCATION_DEBUG "${Live_${UPPERCOMPONENT}_LIBRARY_DEBUG}")
		endif()
	endif()
endforeach()

set(Live_LIBRARIES ""  CACHE LIST "LiveMedia libraries " FORCE)

 foreach( COMPONENT  ${COMPONENTS} )
    string( TOUPPER ${COMPONENT} UPPERCOMPONENT )
   
     list(APPEND Live_LIBRARIES ${Live_${UPPERCOMPONENT}_LIBRARY})
    
  endforeach()
 message(STATUS "Live LIBRARIES : ${Live_LIBRARIES}")
