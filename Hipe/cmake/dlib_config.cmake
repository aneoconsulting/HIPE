cmake_minimum_required(VERSION 3.7.1)

# TODO
# Use find_package support when we get it working with Hipe External.
# find_package(dlib REQUIRED PATHS "${Dlib_DIR}")
# message(STATUS "dlib_INCLUDE_DIRS: ${dlib_INCLUDE_DIRS}")
# message(STATUS "dlib_LIBRARIES: ${dlib_LIBRARIES}")

macro(_Dlib_ADJUST_LIB_VARS basename)

  if(CMAKE_CONFIGURATION_TYPES OR CMAKE_BUILD_TYPE)
    set(Dlib_${basename}_LIBRARY optimized ${Dlib_${basename}_LIBRARY_RELEASE} debug ${Dlib_${basename}_LIBRARY_DEBUG})
  else(CMAKE_CONFIGURATION_TYPES OR CMAKE_BUILD_TYPE)
    # if there are no configuration types and CMAKE_BUILD_TYPE has no value
    # then just use the release libraries
    set(Dlib_${basename}_LIBRARY ${Dlib_${basename}_LIBRARY_DEBUG})
  endif(CMAKE_CONFIGURATION_TYPES OR CMAKE_BUILD_TYPE)
  # FIXME: This probably should be set for both cases
  set(Dlib_${basename}_LIBRARIES optimized ${Dlib_${basename}_LIBRARY_RELEASE} debug ${Dlib_${basename}_LIBRARY_DEBUG})

endmacro(_Dlib_ADJUST_LIB_VARS basename)

set(Dlib_DIR "DLIB-ROOT-NOTFOUND" CACHE PATH "Path to the root dlib directory")
#set(DLIB_LIBRARYDIR "DLIB-LIBRARY-NOTFOUND" CACHE PATH "Path to the static dlib Library")


if(${Dlib_DIR} EQUAL "DLIB-ROOT-NOTFOUND")
  message(FATAL_ERROR "Variable Dlib_DIR is empty")
endif()

set(dlib_INCLUDE_DIRS "${Dlib_DIR}/include" CACHE PATH "include path for dlib" FORCE)

set(Dlib_LIBRARY_DIR "${Dlib_DIR}/lib" CACHE PATH "include path for dlib" FORCE)

set(COMPONENTS dlib)

set(lib_path ${Dlib_LIBRARY_DIR})
set(_lib_list "")


if(WIN32)
  set(EXTENSION .lib)
else(WIN32)
  set(EXTENSION .so)
endif(WIN32)

foreach(COMPONENT  ${COMPONENTS})
  string(TOUPPER ${COMPONENT} UPPERCOMPONENT)
  if(NOT TARGET dlib::${COMPONENT})
    string(TOUPPER ${COMPONENT} UPPERCOMPONENT)
    set(Dlib_${UPPERCOMPONENT}_LIBRARY "")
    set(Dlib_${UPPERCOMPONENT}_LIBRARY_RELEASE "")
    set(Dlib_${UPPERCOMPONENT}_LIBRARY_DEBUG   "")
  endif(NOT TARGET dlib::${COMPONENT})
endforeach(COMPONENT  ${COMPONENTS})

foreach(COMPONENT  ${COMPONENTS})
  if(NOT TARGET dlib::${COMPONENT})
    string(TOUPPER ${COMPONENT} UPPERCOMPONENT)
    message(STATUS "Set LIBRARY : ${COMPONENT}")
    set(Dlib_${UPPERCOMPONENT}_LIBRARY "")

    if(WIN32)
      set(Dlib_${UPPERCOMPONENT}_LIBRARY_RELEASE "${lib_path}/release/${COMPONENT}${EXTENSION}")
      set(Dlib_${UPPERCOMPONENT}_LIBRARY_DEBUG   "${lib_path}/debug/${COMPONENT}${EXTENSION}")
      set(COMMON_DLIB_LIB_DEPS "ws2_32;winmm;comctl32;gdi32;imm32;${HIPE_EXTERNAL_DIR}/intel64_win/mkl_intel_lp64.lib;${HIPE_EXTERNAL_DIR}/intel64_win/mkl_core.lib;${HIPE_EXTERNAL_DIR}/intel64_win/mkl_intel_thread.lib;${HIPE_EXTERNAL_DIR}/intel64_win/libiomp5md.lib;${HIPE_EXTERNAL_DIR}/intel64_win/mkl_intel_lp64.lib;${HIPE_EXTERNAL_DIR}/intel64_win/mkl_core.lib;${HIPE_EXTERNAL_DIR}/intel64_win/mkl_intel_thread.lib;${HIPE_EXTERNAL_DIR}/intel64_win/libiomp5md.lib;C:/Program Files/NVIDIA GPU Computing Toolkit/CUDA/v8.0/lib/x64/cublas.lib;C:/Program Files/NVIDIA GPU Computing Toolkit/CUDA/v8.0/lib/x64/cublas_device.lib;C:/Program Files/NVIDIA GPU Computing Toolkit/CUDA/v8.0/lib/x64/cudnn.lib;C:/Program Files/NVIDIA GPU Computing Toolkit/CUDA/v8.0/lib/x64/curand.lib;C:/Program Files/NVIDIA GPU Computing Toolkit/CUDA/v8.0/lib/x64/cusolver.lib")
      list(APPEND Dlib_${UPPERCOMPONENT}_LIBRARY_RELEASE "${COMMON_DLIB_LIB_DEPS}")
      list(APPEND Dlib_${UPPERCOMPONENT}_LIBRARY_DEBUG "${COMMON_DLIB_LIB_DEPS}")
    endif(WIN32)

    if(UNIX)
      set(Dlib_${UPPERCOMPONENT}_LIBRARY_RELEASE "${lib_path}/lib${COMPONENT}${EXTENSION}")
      set(Dlib_${UPPERCOMPONENT}_LIBRARY_DEBUG   "${lib_path}/lib${COMPONENT}${EXTENSION}")
    endif(UNIX)

    _Dlib_ADJUST_LIB_VARS(${UPPERCOMPONENT})
    add_library(dlib::${COMPONENT} STATIC IMPORTED)
    message(STATUS "Select LIBRARY : ${Dlib_${UPPERCOMPONENT}_LIBRARY}")

    if(EXISTS "${Dlib_${UPPERCOMPONENT}_LIBRARY}")
      set_target_properties(dlib::${COMPONENT} PROPERTIES
      IMPORTED_LINK_INTERFACE_LANGUAGES "CXX"
      IMPORTED_LOCATION "${Dlib_${UPPERCOMPONENT}_LIBRARY}")
    endif(EXISTS "${Dlib_${UPPERCOMPONENT}_LIBRARY}")

    if(EXISTS "${Dlib_${UPPERCOMPONENT}_LIBRARY_RELEASE}")
      set_property(TARGET dlib::${COMPONENT} APPEND PROPERTY
      IMPORTED_CONFIGURATIONS RELEASE)
      set_target_properties(dlib::${COMPONENT} PROPERTIES
      IMPORTED_LINK_INTERFACE_LANGUAGES_RELEASE "CXX"
      IMPORTED_LOCATION_RELEASE "${Dlib_${UPPERCOMPONENT}_LIBRARY_RELEASE}")
    endif(EXISTS "${Dlib_${UPPERCOMPONENT}_LIBRARY_RELEASE}")

    if(EXISTS "${Dlib_${UPPERCOMPONENT}_LIBRARY_DEBUG}")
      set_property(TARGET dlib::${COMPONENT} APPEND PROPERTY
      IMPORTED_CONFIGURATIONS DEBUG)
      set_target_properties(dlib::${COMPONENT} PROPERTIES
      IMPORTED_LINK_INTERFACE_LANGUAGES_DEBUG "CXX"
      IMPORTED_LOCATION_DEBUG "${Dlib_${UPPERCOMPONENT}_LIBRARY_DEBUG}")
    endif(EXISTS "${Dlib_${UPPERCOMPONENT}_LIBRARY_DEBUG}")

  endif(NOT TARGET dlib::${COMPONENT})
endforeach(COMPONENT  ${COMPONENTS})

set(dlib_LIBRARIES ""  CACHE LIST "DlibMedia libraries " FORCE)

foreach(COMPONENT  ${COMPONENTS})
  string(TOUPPER ${COMPONENT} UPPERCOMPONENT)
  list(APPEND dlib_LIBRARIES ${Dlib_${UPPERCOMPONENT}_LIBRARY})
endforeach(COMPONENT  ${COMPONENTS})


message(STATUS "Dlib LIBRARIES : ${dlib_LIBRARIES}")
