

SET(HIPE_EXTERNAL_VERSION "20180409")
SET(FILE_HIPE_EXTERNAL_DEPS "HipeExternal_win64_${HIPE_EXTERNAL_VERSION}.tgz")
set(DEPENDENCIES_SHA "309573da25af25ab3adcc9e7c0e792b3")
if (MSVC)
	if (${USE_PREBUILT_DEPENDENCIES} OR ${FORCE_USE_PREBUILT_DEPENDENCIES})
		message(STATUS "Use prebuilt HIPE EXTERNAL dependencies...")
		if (NOT ${HIPE_EXTERNAL} STREQUAL "")
			message(STATUS "Use prebuilt and HIPE_EXTERNAL EXIST")
			if (NOT EXISTS "${HIPE_EXTERNAL}" OR ${FORCE_USE_PREBUILT_DEPENDENCIES})
				message(STATUS "Download prebuilt HIPE EXTERNAL dependencies...")
				file(MAKE_DIRECTORY "${HIPE_EXTERNAL}")
				file(MAKE_DIRECTORY "${HIPE_EXTERNAL}/win64")
				FILE(DOWNLOAD "ftp://dupihome.ddns.net/HipeExternal/${FILE_HIPE_EXTERNAL_DEPS}" "${HIPE_EXTERNAL}/download/${FILE_HIPE_EXTERNAL_DEPS}"
				USERPWD "public:guest01"
				EXPECTED_MD5 ${DEPENDENCIES_SHA}
				SHOW_PROGRESS
				LOG print_download)
				FILE(MD5 "${HIPE_EXTERNAL}/download/${FILE_HIPE_EXTERNAL_DEPS}" _file_sha)
				if(NOT "${_file_sha}" STREQUAL "${DEPENDENCIES_SHA}")
					message(STATUS "Fail to download archive Checksum differ expected ${DEPENDENCIES_SHA} different of ${_file_sha}")
				else()
					message(STATUS "Download of dependencies OK")
					message(STATUS "Extract Archives to ${HIPE_EXTERNAL}. Please wait...")
					execute_process(
									COMMAND ${CMAKE_COMMAND} -E tar xzf "${HIPE_EXTERNAL}/download/${FILE_HIPE_EXTERNAL_DEPS}"
									WORKING_DIRECTORY "${HIPE_EXTERNAL}/"
									)
					set(FORCE_USE_PREBUILT_DEPENDENCIES OFF CACHE BOOL "Download and use the prebuilt dependencies" FORCE)
				endif()
			endif()
		endif()
	endif()
endif()