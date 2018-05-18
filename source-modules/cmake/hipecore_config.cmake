cmake_minimum_required(VERSION 3.7.1)

# TODO
# Use find_package support when we get it working with Hipe External.
# find_package(hipecore REQUIRED PATHS "${Hipecore_DIR}")
# message(STATUS "hipecore_INCLUDE_DIRS: ${hipecore_INCLUDE_DIRS}")
# message(STATUS "hipecore_LIBRARIES: ${hipecore_LIBRARIES}")

macro(_Hipecore_ADJUST_LIB_VARS basename)

  if(CMAKE_CONFIGURATION_TYPES OR CMAKE_BUILD_TYPE)
    set(Hipecore_${basename}_LIBRARY optimized ${Hipecore_${basename}_LIBRARY_RELEASE} debug ${Hipecore_${basename}_LIBRARY_DEBUG})
  else(CMAKE_CONFIGURATION_TYPES OR CMAKE_BUILD_TYPE)
    # if there are no configuration types and CMAKE_BUILD_TYPE has no value
    # then just use the release libraries
    set(Hipecore_${basename}_LIBRARY ${Hipecore_${basename}_LIBRARY_DEBUG})
  endif(CMAKE_CONFIGURATION_TYPES OR CMAKE_BUILD_TYPE)
  # FIXME: This probably should be set for both cases
  set(Hipecore_${basename}_LIBRARIES optimized ${Hipecore_${basename}_LIBRARY_RELEASE} debug ${Hipecore_${basename}_LIBRARY_DEBUG})

endmacro(_Hipecore_ADJUST_LIB_VARS basename)

set(Hipecore_DIR "HIPECORE-ROOT-NOTFOUND" CACHE PATH "Path to the root hipecore directory")
#set(HIPECORE_LIBRARYDIR "HIPECORE-LIBRARY-NOTFOUND" CACHE PATH "Path to the static hipecore Library")


if(${Hipecore_DIR} EQUAL "HIPECORE-ROOT-NOTFOUND")
  message(FATAL_ERROR "Variable Hipecore_DIR is empty")
endif()

set(hipecore_INCLUDE_DIRS "${Hipecore_DIR}/include" CACHE PATH "include path for hipecore" FORCE)

set(Hipecore_LIBRARY_DIR "${Hipecore_DIR}/lib" CACHE PATH "include path for hipecore" FORCE)

set(COMPONENTS core coredata http json orchestrator corefilter)

set(lib_path ${Hipecore_LIBRARY_DIR})
set(_lib_list "")


if(WIN32)
  set(EXTENSION .lib)
else(WIN32)
  set(EXTENSION_SHARED .so)
  set(EXTENSION_STATIC .a)
endif(WIN32)

foreach(COMPONENT  ${COMPONENTS})
  string(TOUPPER ${COMPONENT} UPPERCOMPONENT)
  if(NOT TARGET hipecore::${COMPONENT})
    string(TOUPPER ${COMPONENT} UPPERCOMPONENT)
    set(Hipecore_${UPPERCOMPONENT}_LIBRARY "")
    set(Hipecore_${UPPERCOMPONENT}_LIBRARY_RELEASE "")
    set(Hipecore_${UPPERCOMPONENT}_LIBRARY_DEBUG   "")
  endif(NOT TARGET hipecore::${COMPONENT})
endforeach(COMPONENT  ${COMPONENTS})

foreach(COMPONENT  ${COMPONENTS})
  if(NOT TARGET hipecore::${COMPONENT})
    string(TOUPPER ${COMPONENT} UPPERCOMPONENT)
    message(STATUS "Set LIBRARY : ${COMPONENT}")
    set(Hipecore_${UPPERCOMPONENT}_LIBRARY "")

    if(WIN32)
      set(Hipecore_${UPPERCOMPONENT}_LIBRARY_RELEASE "${lib_path}/Release/${COMPONENT}${EXTENSION}")
      set(Hipecore_${UPPERCOMPONENT}_LIBRARY_DEBUG   "${lib_path}/Debug/${COMPONENT}${EXTENSION}")
      set(COMMON_HIPECORE_LIB_DEPS "ws2_32;winmm;comctl32;gdi32;imm32;${HIPE_EXTERNAL_DIR}/intel64_win/mkl_intel_lp64.lib;${HIPE_EXTERNAL_DIR}/intel64_win/mkl_core.lib;${HIPE_EXTERNAL_DIR}/intel64_win/mkl_intel_thread.lib;${HIPE_EXTERNAL_DIR}/intel64_win/libiomp5md.lib;${HIPE_EXTERNAL_DIR}/intel64_win/mkl_intel_lp64.lib;${HIPE_EXTERNAL_DIR}/intel64_win/mkl_core.lib;${HIPE_EXTERNAL_DIR}/intel64_win/mkl_intel_thread.lib;${HIPE_EXTERNAL_DIR}/intel64_win/libiomp5md.lib;C:/Program Files/NVIDIA GPU Computing Toolkit/CUDA/v8.0/lib/x64/cublas.lib;C:/Program Files/NVIDIA GPU Computing Toolkit/CUDA/v8.0/lib/x64/cublas_device.lib;C:/Program Files/NVIDIA GPU Computing Toolkit/CUDA/v8.0/lib/x64/cudnn.lib;C:/Program Files/NVIDIA GPU Computing Toolkit/CUDA/v8.0/lib/x64/curand.lib;C:/Program Files/NVIDIA GPU Computing Toolkit/CUDA/v8.0/lib/x64/cusolver.lib")
      list(APPEND Hipecore_${UPPERCOMPONENT}_LIBRARY_RELEASE "${COMMON_HIPECORE_LIB_DEPS}")
      list(APPEND Hipecore_${UPPERCOMPONENT}_LIBRARY_DEBUG "${COMMON_HIPECORE_LIB_DEPS}")
    endif(WIN32)

    if(UNIX)
      set(Hipecore_${UPPERCOMPONENT}_LIBRARY_RELEASE "${lib_path}/Release/lib${COMPONENT}${EXTENSION_SHARED}")
	  if (NOT EXISTS ${Hipecore_${UPPERCOMPONENT}_LIBRARY_RELEASE})
		set(Hipecore_${UPPERCOMPONENT}_LIBRARY_RELEASE "${lib_path}/Release/lib${COMPONENT}${EXTENSION_STATIC}")
	  endif()

	  set(Hipecore_${UPPERCOMPONENT}_LIBRARY_DEBUG   "${lib_path}/Debug/lib${COMPONENT}${EXTENSION_SHARED}")
	  if (NOT EXISTS ${Hipecore_${UPPERCOMPONENT}_LIBRARY_DEBUG})
		set(Hipecore_${UPPERCOMPONENT}_LIBRARY_DEBUG   "${lib_path}/Debug/lib${COMPONENT}${EXTENSION_STATIC}")
	  endif()
	  
    endif(UNIX)

    _Hipecore_ADJUST_LIB_VARS(${UPPERCOMPONENT})
    add_library(hipecore::${COMPONENT} STATIC IMPORTED)
    message(STATUS "Select LIBRARY : ${Hipecore_${UPPERCOMPONENT}_LIBRARY}")

    if(EXISTS "${Hipecore_${UPPERCOMPONENT}_LIBRARY}")
      set_target_properties(hipecore::${COMPONENT} PROPERTIES
      IMPORTED_LINK_INTERFACE_LANGUAGES "CXX"
      IMPORTED_LOCATION "${Hipecore_${UPPERCOMPONENT}_LIBRARY}")
    endif(EXISTS "${Hipecore_${UPPERCOMPONENT}_LIBRARY}")

    if(EXISTS "${Hipecore_${UPPERCOMPONENT}_LIBRARY_RELEASE}")
      set_property(TARGET hipecore::${COMPONENT} APPEND PROPERTY
      IMPORTED_CONFIGURATIONS RELEASE)
      set_target_properties(hipecore::${COMPONENT} PROPERTIES
      IMPORTED_LINK_INTERFACE_LANGUAGES_RELEASE "CXX"
      IMPORTED_LOCATION_RELEASE "${Hipecore_${UPPERCOMPONENT}_LIBRARY_RELEASE}")
    endif(EXISTS "${Hipecore_${UPPERCOMPONENT}_LIBRARY_RELEASE}")

    if(EXISTS "${Hipecore_${UPPERCOMPONENT}_LIBRARY_DEBUG}")
      set_property(TARGET hipecore::${COMPONENT} APPEND PROPERTY
      IMPORTED_CONFIGURATIONS DEBUG)
      set_target_properties(hipecore::${COMPONENT} PROPERTIES
      IMPORTED_LINK_INTERFACE_LANGUAGES_DEBUG "CXX"
      IMPORTED_LOCATION_DEBUG "${Hipecore_${UPPERCOMPONENT}_LIBRARY_DEBUG}")
    endif(EXISTS "${Hipecore_${UPPERCOMPONENT}_LIBRARY_DEBUG}")

  endif(NOT TARGET hipecore::${COMPONENT})
endforeach(COMPONENT  ${COMPONENTS})

set(hipecore_LIBRARIES ""  CACHE LIST "HipeCore libraries " FORCE)

foreach(COMPONENT  ${COMPONENTS})
  string(TOUPPER ${COMPONENT} UPPERCOMPONENT)
  list(APPEND hipecore_LIBRARIES ${Hipecore_${UPPERCOMPONENT}_LIBRARY})
endforeach(COMPONENT  ${COMPONENTS})


message(STATUS "HipeCore LIBRARIES : ${hipecore_LIBRARIES}")
prepend_include_directories_if_necessary("${hipecore_INCLUDE_DIRS}")
