# - Try to find Openblas
#
# The following variables are optionally searched for defaults
#  OPENBLAS_ROOT_DIR:            Base directory where all OPENBLAS components are found
#
# The following are set after configuration is done:
#  OPENBLAS_FOUND
#  OPENBLAS_INCLUDE_DIRS
#  OPENBLAS_LIBRARIES
#  OPENBLAS_LIBRARYRARY_DIRS

include(FindPackageHandleStandardArgs)

set(OPENBLAS_ROOT_DIR "" CACHE PATH "Folder contains Google glog")
message(STATUS "Find Openblas please ${OPENBLAS_ROOT_DIR}")

if(WIN32)
    find_path(OPENBLAS_INCLUDE_DIR cblas.h
        PATHS ${OPENBLAS_ROOT_DIR}/include)
else()
    find_path(OPENBLAS_INCLUDE_DIR cblas.h
        PATHS ${OPENBLAS_ROOT_DIR})
endif()

if(MSVC)
    # find_library(OPENBLAS_LIBRARY_RELEASE NAMES libopenblas.a
        # PATHS "${OPENBLAS_ROOT_DIR}/lib"
        # )
	set(OPENBLAS_LIBRARY_RELEASE "${OPENBLAS_ROOT_DIR}/lib/libopenblas.dll.a" CACHE PATH "OPENBLAS LIB Release" FORCE)
    # find_library(OPENBLAS_LIBRARY_DEBUG NAMES libopenblas.a
        # PATHS "${OPENBLAS_ROOT_DIR}/lib"
        # )
	set(OPENBLAS_LIBRARY_DEBUG "${OPENBLAS_ROOT_DIR}/lib/libopenblas.dll.a" CACHE PATH "OPENBLAS LIB Debug" FORCE)
	
    set(OPENBLAS_LIBRARY optimized ${OPENBLAS_LIBRARY_RELEASE} debug ${OPENBLAS_LIBRARY_DEBUG})
else()
    find_library(OPENBLAS_LIBRARY openblas
        PATHS ${OPENBLAS_ROOT_DIR}
        PATH_SUFFIXES lib lib64)
endif()

find_package_handle_standard_args(Openblas DEFAULT_MSG OPENBLAS_INCLUDE_DIR OPENBLAS_LIBRARY)

if(OPENBLAS_FOUND)
  set(OPENBLAS_INCLUDE_DIRS ${OPENBLAS_INCLUDE_DIR})
  set(OPENBLAS_LIBRARIES ${OPENBLAS_LIBRARY})
  message(STATUS "Found openblas    (include: ${OPENBLAS_INCLUDE_DIR}, library: ${OPENBLAS_LIBRARY})")
  mark_as_advanced(OPENBLAS_ROOT_DIR OPENBLAS_LIBRARY_RELEASE OPENBLAS_LIBRARY_DEBUG
                                 OPENBLAS_LIBRARY OPENBLAS_INCLUDE_DIR)
endif()