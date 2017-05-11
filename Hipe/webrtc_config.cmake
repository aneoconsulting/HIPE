cmake_minimum_required (VERSION 3.7.1)

macro(_webrtc_ADJUST_LIB_VARS basename)
	
	if(CMAKE_CONFIGURATION_TYPES OR CMAKE_BUILD_TYPE)
        set(webrtc_${basename}_LIBRARY optimized ${webrtc_${basename}_LIBRARY_RELEASE} debug ${webrtc_${basename}_LIBRARY_DEBUG})
    else()
        # if there are no configuration types and CMAKE_BUILD_TYPE has no value
        # then just use the release libraries
        set(webrtc_${basename}_LIBRARY ${webrtc_${basename}_LIBRARY_DEBUG} )
    endif()
      # FIXME: This probably should be set for both cases
    set(webrtc_${basename}_LIBRARIES optimized ${webrtc_${basename}_LIBRARY_RELEASE} debug ${webrtc_${basename}_LIBRARY_DEBUG})
	
endmacro()

set(WEBRTC_DIR "webrtc-ROOT-NOTFOUND" CACHE PATH "Path to the root webrtc directory" )
set(webrtc_LIBRARYDIR "webrtc-LIBRARY-NOTFOUND" CACHE PATH "Path to the static webrtc Library" )


if (${WEBRTC_DIR} EQUAL "webrtc-ROOT-NOTFOUND")
	message( FATAL_ERROR "Variable WEBRTC_DIR is empty")
endif()

set(webrtc_INCLUDEDIR "${WEBRTC_DIR}/include;${WEBRTC_DIR}/include/third_party/boringssl/src/include" CACHE PATH "include path for webrtc" FORCE)

if (WIN32)
  set(webrtc_LIBRARYDIR "${WEBRTC_DIR}" CACHE PATH "include path for webrtc" FORCE)
endif()

if(UNIX)
  set(webrtc_LIBRARYDIR "${WEBRTC_DIR}" CACHE PATH "include path for webrtc" FORCE)
endif()



set(lib_path ${webrtc_LIBRARYDIR})
set(_lib_list "")


if (WIN32)
set(EXTENSION .lib)
else()
set(EXTENSION .so)
endif()

#SET(COMPONENTS "")
FILE(GLOB_RECURSE COMPONENTS 
	LIST_DIRECTORIES false 
	RELATIVE "${webrtc_LIBRARYDIR}/out/x64/Debug/"
    "${webrtc_LIBRARYDIR}/out/x64/Debug/*${EXTENSION}")

foreach( COMPONENT  ${COMPONENTS} )

	string( TOUPPER ${COMPONENT} UPPERCOMPONENT )
	string(FIND ${UPPERCOMPONENT} "LITE" ignore_proto)
	if (${ignore_proto} GREATER 0 )
		message(STATUS "IGNORE LIBRARIES : ${ignore_proto}")
		continue()
	endif()
	
	
	if (NOT TARGET webrtc::${COMPONENT})
		string( TOUPPER ${COMPONENT} UPPERCOMPONENT )
		set(webrtc_${UPPERCOMPONENT}_LIBRARY "")
		set(webrtc_${UPPERCOMPONENT}_LIBRARY_RELEASE "")
		set(webrtc_${UPPERCOMPONENT}_LIBRARY_DEBUG   "")
	endif()
endforeach()

foreach( COMPONENT  ${COMPONENTS} )
	if (NOT TARGET webrtc::${COMPONENT})
	string( TOUPPER ${COMPONENT} UPPERCOMPONENT )
	string(FIND ${UPPERCOMPONENT} "LITE" ignore_proto)
	if (${ignore_proto} GREATER 0 )
		message(STATUS "IGNORE LIBRARIES : ${ignore_proto}")
		continue()
	endif()
	
	
		message(STATUS "Set LIBRARY : ${COMPONENT}")
		set(webrtc_${UPPERCOMPONENT}_LIBRARY "")
		if (WIN32)
		  set(webrtc_${UPPERCOMPONENT}_LIBRARY_RELEASE "${lib_path}/out/x64/Release/${COMPONENT}")
		  set(webrtc_${UPPERCOMPONENT}_LIBRARY_DEBUG   "${lib_path}/out/x64/Debug/${COMPONENT}")
		endif()

		if(UNIX)
		  set(webrtc_${UPPERCOMPONENT}_LIBRARY_RELEASE "${lib_path}/lib${COMPONENT}")
		  set(webrtc_${UPPERCOMPONENT}_LIBRARY_DEBUG   "${lib_path}/lib${COMPONENT}")
		endif()
		
		_webrtc_ADJUST_LIB_VARS(${UPPERCOMPONENT})
		add_library(webrtc::${COMPONENT} SHARED IMPORTED)
		message(STATUS "Select LIBRARY : ${webrtc_${UPPERCOMPONENT}_LIBRARY}")
		
		if(EXISTS "${webrtc_${UPPERCOMPONENT}_LIBRARY}")
			  set_target_properties(webrtc::${COMPONENT} PROPERTIES
				IMPORTED_LINK_INTERFACE_LANGUAGES "CXX"
				IMPORTED_LOCATION "${webrtc_${UPPERCOMPONENT}_LIBRARY}")
		endif()
		if(EXISTS "${webrtc_${UPPERCOMPONENT}_LIBRARY_RELEASE}")
		  set_property(TARGET webrtc::${COMPONENT} APPEND PROPERTY
			IMPORTED_CONFIGURATIONS RELEASE)
		  set_target_properties(webrtc::${COMPONENT} PROPERTIES
			IMPORTED_LINK_INTERFACE_LANGUAGES_RELEASE "CXX"
			IMPORTED_LOCATION_RELEASE "${webrtc_${UPPERCOMPONENT}_LIBRARY_RELEASE}")
		endif()
		if(EXISTS "${webrtc_${UPPERCOMPONENT}_LIBRARY_DEBUG}")
		  set_property(TARGET webrtc::${COMPONENT} APPEND PROPERTY
			IMPORTED_CONFIGURATIONS DEBUG)
		  set_target_properties(webrtc::${COMPONENT} PROPERTIES
			IMPORTED_LINK_INTERFACE_LANGUAGES_DEBUG "CXX"
			IMPORTED_LOCATION_DEBUG "${webrtc_${UPPERCOMPONENT}_LIBRARY_DEBUG}")
		endif()
	endif()
endforeach()

set(webrtc_LIBRARIES ""  CACHE LIST "webrtcMedia libraries " FORCE)

 foreach( COMPONENT  ${COMPONENTS} )
    string( TOUPPER ${COMPONENT} UPPERCOMPONENT )
   
     list(APPEND webrtc_LIBRARIES ${webrtc_${UPPERCOMPONENT}_LIBRARY})
    
  endforeach()
 message(STATUS "webrtc LIBRARIES : ${webrtc_LIBRARIES}")
