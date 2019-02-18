function(display_pathlist _header _paths)
  set(_list_separator "\n--   ")
  string(REPLACE ";" "${_list_separator}" _path_list "${_paths}")
  message(STATUS "${_header}:${_list_separator}${_path_list}")
endfunction(display_pathlist _paths)



function(display_pathlist_property _target _property_name)
  get_target_property(_property_value ${_target} "${_property_name}")
  display_pathlist("${_target} ${_property_name}" "${_property_value}")
endfunction(display_pathlist_property _target _property)



function(display_target_properties _target)
  display_pathlist_property("${_target}" "INCLUDE_DIRECTORIES")
  display_pathlist_property("${_target}" "LINK_LIBRARIES")
endfunction(display_target_properties _target)


# Prepend target include directories if they are not already present. This
# ensures that include directories in Hipe External take precedence without
# the risk of shadowing by system directories when the user chooses to compile
# with one or more optional host libraries.
# This function should only be used with system directories as it will pass
# "SYSTEM" to the target_include_directories command.
function(prepend_target_include_directories_if_necessary _target _include_dirs)
  get_target_property(_target_include_dirs ${_target} INCLUDE_DIRECTORIES)
  set(_rev_include_dirs "${_include_dirs}")
  list(REVERSE _rev_include_dirs)
  foreach(_include_dir ${_rev_include_dirs})
    if(NOT "${_include_dir}" IN_LIST _target_include_dirs)
      target_include_directories(${_target} SYSTEM BEFORE PRIVATE ${_include_dir})
    endif(NOT "${_include_dir}" IN_LIST _target_include_dirs)
  endforeach(_include_dir ${_rev_include_dirs})
endfunction(prepend_target_include_directories_if_necessary _target _include_dirs)


# Same as prepend_target_include_directories_if_necessary but for the whole
# project.
function(prepend_include_directories_if_necessary _include_dirs)
  set(_rev_include_dirs "${_include_dirs}")
  list(REVERSE _rev_include_dirs)
  foreach(_include_dir ${_rev_include_dirs})
    if(NOT "${_include_dir}" IN_LIST INCLUDE_DIRECTORIES)
      include_directories(BEFORE SYSTEM ${_include_dir})
    endif(NOT "${_include_dir}" IN_LIST INCLUDE_DIRECTORIES)
  endforeach(_include_dir ${_rev_include_dirs})
endfunction(prepend_include_directories_if_necessary _include_dirs)



MACRO(SUBDIRLIST result curdir)
  FILE(GLOB children RELATIVE ${curdir} ${curdir}/*)
  SET(dirlist "")
  FOREACH(child ${children})
    IF(IS_DIRECTORY ${curdir}/${child})
      LIST(APPEND dirlist ${child})
    ENDIF()
  ENDFOREACH()
  SET(${result} ${dirlist})
ENDMACRO()


# Create source groups for visual studio.
function(group_for_visual_studio _subgroup _root_dir _file_list)
  foreach(_file_path IN ITEMS ${_file_list})
    message(STATUS "grouping for visual studio: ${_file_path}")
    # Get the directory. Replace PATH with DIRECTORY for CMake > 2.8.11
    get_filename_component(_dir_path "${_file_path}" PATH)
    file(RELATIVE_PATH _dir_path_rel "${_root_dir}" "${_dir_path}")
    string(REPLACE "/" "\\" _group_path "${_dir_path_rel}")
    source_group("${_subgroup}\\${_group_path}" FILES "${_file_path}")
  endforeach(_file_path IN ITEMS ${_file_list})
endfunction(group_for_visual_studio)



function(message_header header_text)
  string(REGEX REPLACE "." "#" _hash_spacer "${header_text}")
  message(
    STATUS
    "\n\
-- ######${_hash_spacer}######\n\
-- #     ${header_text}     #\n\
-- ######${_hash_spacer}######"
  )
endfunction(message_header)


function(CUDA_CONVERT_FLAGS EXISTING_TARGET)
    get_property(old_flags TARGET ${EXISTING_TARGET} PROPERTY INTERFACE_COMPILE_OPTIONS)
	message(status "All_Flags : ${old_flags}")
    if(NOT "${old_flags}" STREQUAL "")
        string(REPLACE ";" "," CUDA_flags "${old_flags}")
        set_property(TARGET ${EXISTING_TARGET} PROPERTY INTERFACE_COMPILE_OPTIONS
            "$<$<BUILD_INTERFACE:$<COMPILE_LANGUAGE:CXX>>:${old_flags}>$<$<BUILD_INTERFACE:$<COMPILE_LANGUAGE:CUDA>>:-Xcompiler=${CUDA_flags}>"
            )
    endif()
endfunction()

# Regroup all common commands in a macro for building HIPE libraries to avoid
# redundancy.

# TODO
# Refactor. The current version is directly inhereted and was only slightly
# modified to create the macro.

macro(add_hipe_library _hipe_lib_name module_path _lib_type)
  message_header("HIPE LIBRARY: ${_hipe_lib_name}")

  set(_src_lib_path ${CMAKE_CURRENT_SOURCE_DIR})
  set(_src_lib_inc_path ${CMAKE_SOURCE_DIR}/${module_path}/header/${_hipe_lib_name})

  message(STATUS "${_hipe_lib_name} source path: ${_src_lib_path}" )

  file(
    GLOB_RECURSE _source_list
    LIST_DIRECTORIES false
    "${_src_lib_path}/*.c*"
  )

  file(
    GLOB_RECURSE _header_list
    LIST_DIRECTORIES false
    "${_src_lib_inc_path}/*.h*"
    "${_src_lib_inc_path}/*.cuh*"
  )

  # LIBRARY
#   if(HIPE_STATIC_LIBS)
#     set(_lib_type "STATIC")
#   else(HIPE_STATIC_LIBS)
#     set(_lib_type "SHARED")
#   endif(HIPE_STATIC_LIBS)

  add_library(${_hipe_lib_name} ${_lib_type} ${_source_list} ${_header_list})
  target_include_directories(${_hipe_lib_name} BEFORE PRIVATE ${_src_lib_inc_path})
  target_include_directories(${_hipe_lib_name} BEFORE PRIVATE ${_src_lib_inc_path}/../)

  if (UNIX)
    set_property(TARGET ${_hipe_lib_name} PROPERTY POSITION_INDEPENDENT_CODE ON)
  #   target_compile_options(${_hipe_lib_name} PRIVATE -fPIC)
  endif(UNIX)



  # Group source files for Visual Studio
  group_for_visual_studio("source" "${_src_lib_path}" "${_source_list}")
  group_for_visual_studio("header" "${_src_lib_inc_path}" "${_header_list}")

endmacro(add_hipe_library)


# Variant of add_hipe_library for executables. See notes for add_hipe_library.
macro(add_hipe_executable _hipe_exe_name module_path)
  message_header("HIPE LIBRARY: ${_hipe_exe_name}")

  set(_src_exe_path ${CMAKE_CURRENT_SOURCE_DIR})
  set(_src_exe_inc_path ${CMAKE_SOURCE_DIR}/${module_path}/header/${_hipe_exe_name})

  message(STATUS "${_hipe_exe_name} source path: ${_src_exe_path}" )

  file(
    GLOB_RECURSE _source_list
    LIST_DIRECTORIES false
    "${_src_exe_path}/*.c*"
  )

  file(
    GLOB_RECURSE _header_list
    LIST_DIRECTORIES false
    "${_src_exe_inc_path}/*.h*"
    "${_src_exe_inc_path}/*.cuh*"
  )

  add_executable(${_hipe_exe_name} ${_source_list} ${_header_list})
  target_include_directories(${_hipe_exe_name} BEFORE PRIVATE ${_src_exe_inc_path})

  if(HIPE_STATIC_EXES)
    target_sources(${_hipe_exe_name} PUBLIC "-static")
  endif(HIPE_STATIC_EXES)

  if (UNIX)
    # TODO
    # Remove this or make it an option. Suffixes are normally omitted from
    # executable binaries on Linux.
    set_target_properties(${_hipe_exe_name} PROPERTIES SUFFIX ".bin")
  endif(UNIX)



  # Group source files for Visual Studio
  group_for_visual_studio("source" "${_src_exe_path}" "${_source_list}")
  group_for_visual_studio("header" "${_src_exe_inc_path}" "${_header_list}")

endmacro(add_hipe_executable)



# Helper function to add preprocesor definition of FILE_BASENAME
# to pass the filename without directory path for debugging use.
#
# Example:
#
#   define_file_basename_for_sources(my_target)
#
# Will add -DFILE_BASENAME="filename" for each source file depended on
# by my_target, where filename is the name of the file.
#
function(define_file_basename_for_sources targetname)
  get_target_property(source_files "${targetname}" SOURCES)
  foreach(sourcefile ${source_files})
    # Get source file's current list of compile definitions.
    get_property(defs SOURCE "${sourcefile}" PROPERTY COMPILE_DEFINITIONS)
    # Add the FILE_BASENAME=filename compile definition to the list.
    get_filename_component(basename "${sourcefile}" NAME)
    string(REGEX MATCH "^(.*)\\.[^.]*$" dummy ${basename})
    set(basename_without_ext ${CMAKE_MATCH_1})
    list(APPEND defs "FILE_BASENAME=${basename_without_ext}")
    # Set the updated compile definitions on the source file.
    message(STATUS "${sourcefile}: FILE_BASENAME=${basename_without_ext}")
    set_property(
      SOURCE "${sourcefile}"
      PROPERTY COMPILE_DEFINITIONS ${defs}
    )
  endforeach(sourcefile ${source_files})
endfunction()


macro(install_header_to_target targetname)
	set(_src_filter_inc_path ${CMAKE_SOURCE_DIR}/header/${targetname})
	file(
	   GLOB_RECURSE ${targetname}_header
	   LIST_DIRECTORIES false
	   RELATIVE "${_src_filter_inc_path}"
	   "${_src_filter_inc_path}/*.h*"
	 )
	 message(status "Header in ${targetname} : [${${targetname}_header}]")
	foreach ( file ${${targetname}_header} )

		get_filename_component( dir ${file} DIRECTORY )
		install( FILES ${CMAKE_SOURCE_DIR}/header/${targetname}/${file} DESTINATION include/${targetname}/${dir} )
	endforeach()
	
endmacro()

macro(install_dependencies_int target_name EXT_BIN)


if(CMAKE_SYSTEM_PROCESSOR MATCHES "^(powerpc|ppc)64le")
	install(CODE "set(IS_POWERPC ON)" COMPONENT runtime)
endif()

	install(CODE "set(target_name \"${target_name}\")" COMPONENT runtime)
	install(CODE "set(EXT_BIN \"${EXT_BIN}\")" COMPONENT runtime)
	install(CODE "set(HIPE_EXTERNAL_DIR \"${HIPE_EXTERNAL_DIR}\")" COMPONENT runtime)
	install(CODE "set(PROJECT_SOURCE_DIR \"${PROJECT_SOURCE_DIR}\")" COMPONENT runtime)
	install(CODE "set(Hipecore_DIR \"${Hipecore_DIR}\")" COMPONENT runtime)
	install(CODE "set(CUDA_TOOLKIT_ROOT_DIR \"${CUDA_TOOLKIT_ROOT_DIR}\")" COMPONENT runtime)
	install(SCRIPT "${PROJECT_SOURCE_DIR}/../../cmake/installConfig.cmake" COMPONENT runtime)
	file(TO_NATIVE_PATH "${HIPE_EXTERNAL_DIR}" HIPE_EXTERNAL_DIR)
	if (WIN32)
		set(SCRIPT_EXT ".bat")
	else()
		set(SCRIPT_EXT ".sh")
	endif()
	
endmacro(install_dependencies_int)

macro(install_dependencies target_name)

	set(EXT_BIN .so) 
	if (WIN32)
	set(EXT_BIN .dll)
	endif() 
	
	install_dependencies_int(${target_name} ${EXT_BIN})
endmacro(install_dependencies)
