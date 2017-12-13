cmake_minimum_required(VERSION 3.7.1)

set(COMPONENTS uv archo av base crypto http json net sched socketio stun symple turn util uv webrtc)


set(EXT_COMPONENTS http_parser minizip libuv)
set(sourcey_EXT_LIBRARIES "")


macro(_sourcey_ADJUST_LIB_VARS basename)
  if(CMAKE_CONFIGURATION_TYPES OR CMAKE_BUILD_TYPE)
    set(sourcey_${basename}_LIBRARY optimized ${sourcey_${basename}_LIBRARY_RELEASE} debug ${sourcey_${basename}_LIBRARY_DEBUG})
  else(CMAKE_CONFIGURATION_TYPES OR CMAKE_BUILD_TYPE)
    # if there are no configuration types and CMAKE_BUILD_TYPE has no value
    # then just use the release libraries
    set(sourcey_${basename}_LIBRARY ${sourcey_${basename}_LIBRARY_DEBUG})
  endif(CMAKE_CONFIGURATION_TYPES OR CMAKE_BUILD_TYPE)
  # FIXME: This probably should be set for both cases
  set(sourcey_${basename}_LIBRARIES optimized ${sourcey_${basename}_LIBRARY_RELEASE} debug ${sourcey_${basename}_LIBRARY_DEBUG})
endmacro(_sourcey_ADJUST_LIB_VARS basename)

set(SOURCEY_DIR "sourcey-ROOT-NOTFOUND" CACHE PATH "Path to the root sourcey directory")
set(sourcey_LIBRARYDIR "sourcey-LIBRARY-NOTFOUND" CACHE PATH "Path to the static sourcey Library")


if(${SOURCEY_DIR} EQUAL "sourcey-ROOT-NOTFOUND")
  message(FATAL_ERROR "Variable SOURCEY_DIR is empty")
endif(${SOURCEY_DIR} EQUAL "sourcey-ROOT-NOTFOUND")

set(sourcey_INCLUDEDIR "${SOURCEY_DIR}/include;${SOURCEY_DIR}/share/scy/vendor/include" CACHE PATH "include path for sourcey" FORCE)

if(WIN32)
  set(sourcey_LIBRARYDIR "${SOURCEY_DIR}/lib" CACHE PATH "include path for sourcey" FORCE)
  set(sourcey_EXT_LIBRARYDIR "${SOURCEY_DIR}/share/scy/vendor/lib" CACHE PATH "include external lib path for sourcey" FORCE)
endif(WIN32)

if(UNIX)
  set(sourcey_LIBRARYDIR "${SOURCEY_DIR}/lib" CACHE PATH "include path for sourcey" FORCE)
endif(UNIX)


set(lib_path ${sourcey_LIBRARYDIR})
set(_lib_list "")


if(WIN32)
  set(EXTENSION .lib)
else(WIN32)
  set(EXTENSION .a)
endif(WIN32)


foreach(COMPONENT ${EXT_COMPONENTS})
  list(APPEND sourcey_EXT_LIBRARIES optimized ${sourcey_EXT_LIBRARYDIR}/${COMPONENT}${EXTENSION} debug ${sourcey_EXT_LIBRARYDIR}/${COMPONENT}d${EXTENSION})
endforeach(COMPONENT ${EXT_COMPONENTS})


foreach(COMPONENT  ${COMPONENTS})
  string(TOUPPER ${COMPONENT} UPPERCOMPONENT)
  if(NOT TARGET sourcey::${COMPONENT})
    string(TOUPPER ${COMPONENT} UPPERCOMPONENT)
    set(sourcey_${UPPERCOMPONENT}_LIBRARY "")
    set(sourcey_${UPPERCOMPONENT}_LIBRARY_RELEASE "")
    set(sourcey_${UPPERCOMPONENT}_LIBRARY_DEBUG   "")
  endif(NOT TARGET sourcey::${COMPONENT})
endforeach(COMPONENT  ${COMPONENTS})

foreach(COMPONENT  ${COMPONENTS})
  if(NOT TARGET sourcey::${COMPONENT})
  string(TOUPPER ${COMPONENT} UPPERCOMPONENT)
    message(STATUS "Set LIBRARY : ${COMPONENT}")
    set(sourcey_${UPPERCOMPONENT}_LIBRARY "")
    if(WIN32)
      set(sourcey_${UPPERCOMPONENT}_LIBRARY_RELEASE "${lib_path}/scy_${COMPONENT}${EXTENSION}")
      set(sourcey_${UPPERCOMPONENT}_LIBRARY_DEBUG   "${lib_path}/scy_${COMPONENT}d${EXTENSION}")
    endif(WIN32)

    if(UNIX)
      set(sourcey_${UPPERCOMPONENT}_LIBRARY_RELEASE "${lib_path}/libscy_${COMPONENT}${EXTENSION}")
      set(sourcey_${UPPERCOMPONENT}_LIBRARY_DEBUG   "${lib_path}/libscy_${COMPONENT}d${EXTENSION}")
    endif(UNIX)

    _sourcey_ADJUST_LIB_VARS(${UPPERCOMPONENT})
    add_library(sourcey::${COMPONENT} SHARED IMPORTED)
    message(STATUS "Select LIBRARY : ${sourcey_${UPPERCOMPONENT}_LIBRARY}")

    if(EXISTS "${sourcey_${UPPERCOMPONENT}_LIBRARY}")
        set_target_properties(sourcey::${COMPONENT} PROPERTIES
        IMPORTED_LINK_INTERFACE_LANGUAGES "CXX"
        IMPORTED_LOCATION "${sourcey_${UPPERCOMPONENT}_LIBRARY}")
    endif(EXISTS "${sourcey_${UPPERCOMPONENT}_LIBRARY}")

    if(EXISTS "${sourcey_${UPPERCOMPONENT}_LIBRARY_RELEASE}")
      set_property(TARGET sourcey::${COMPONENT} APPEND PROPERTY
      IMPORTED_CONFIGURATIONS RELEASE)
      set_target_properties(sourcey::${COMPONENT} PROPERTIES
      IMPORTED_LINK_INTERFACE_LANGUAGES_RELEASE "CXX"
      IMPORTED_LOCATION_RELEASE "${sourcey_${UPPERCOMPONENT}_LIBRARY_RELEASE}")
    endif(EXISTS "${sourcey_${UPPERCOMPONENT}_LIBRARY_RELEASE}")

    if(EXISTS "${sourcey_${UPPERCOMPONENT}_LIBRARY_DEBUG}")
      set_property(TARGET sourcey::${COMPONENT} APPEND PROPERTY
      IMPORTED_CONFIGURATIONS DEBUG)
      set_target_properties(sourcey::${COMPONENT} PROPERTIES
      IMPORTED_LINK_INTERFACE_LANGUAGES_DEBUG "CXX"
      IMPORTED_LOCATION_DEBUG "${sourcey_${UPPERCOMPONENT}_LIBRARY_DEBUG}")
    endif(EXISTS "${sourcey_${UPPERCOMPONENT}_LIBRARY_DEBUG}")

  endif(NOT TARGET sourcey::${COMPONENT})
endforeach(COMPONENT  ${COMPONENTS})


set(sourcey_LIBRARIES ""  CACHE LIST "sourceyMedia libraries " FORCE)

foreach(COMPONENT  ${COMPONENTS})
  string(TOUPPER ${COMPONENT} UPPERCOMPONENT)
  list(APPEND sourcey_LIBRARIES ${sourcey_${UPPERCOMPONENT}_LIBRARY})
endforeach(COMPONENT  ${COMPONENTS})

list(APPEND sourcey_LIBRARIES ${sourcey_EXT_LIBRARIES})
message(STATUS "sourcey LIBRARIES : ${sourcey_LIBRARIES}")
