

SET(HIPE_EXTERNAL_VERSION "20181119")
SET(FILE_HIPE_EXTERNAL_DEPS "HipeExternal_win64_${HIPE_EXTERNAL_VERSION}.tgz")
set(DEPENDENCIES_SHA "0f55d02b88014cbb87eb52302997fdc4")
if (MSVC)
	if (${USE_PREBUILT_DEPENDENCIES} OR ${FORCE_USE_PREBUILT_DEPENDENCIES})
		message(STATUS "Use prebuilt HIPE EXTERNAL dependencies...")
		if (NOT ${HIPE_EXTERNAL} STREQUAL "")
			message(STATUS "Use prebuilt and HIPE_EXTERNAL EXIST")
			if (NOT EXISTS "${HIPE_EXTERNAL}" OR ${FORCE_USE_PREBUILT_DEPENDENCIES})
				message(STATUS "Check if prebuilt HIPE EXTERNAL dependencies exists")
				file(MAKE_DIRECTORY "${HIPE_EXTERNAL}")
				
				file(MAKE_DIRECTORY "${HIPE_EXTERNAL}/download")
				SET(_file_download_md5 "0")
				if (EXISTS "${HIPE_EXTERNAL}/download/${FILE_HIPE_EXTERNAL_DEPS}")
					FILE(MD5 "${HIPE_EXTERNAL}/download/${FILE_HIPE_EXTERNAL_DEPS}" _file_download_md5)
				endif()
				if(NOT "${_file_download_md5}" STREQUAL "${DEPENDENCIES_SHA}")
					message(STATUS "Need to download prebuilt HIPE EXTERNAL dependencies...")
					FILE(DOWNLOAD "ftp://dupihome.ddns.net/HipeExternal/${FILE_HIPE_EXTERNAL_DEPS}" "${HIPE_EXTERNAL}/download/${FILE_HIPE_EXTERNAL_DEPS}"
					#FILE(DOWNLOAD "ftp://192.168.1.107/HipeExternal/${FILE_HIPE_EXTERNAL_DEPS}" "${HIPE_EXTERNAL}/download/${FILE_HIPE_EXTERNAL_DEPS}"
					USERPWD "public:guest01"
					EXPECTED_MD5 ${DEPENDENCIES_SHA}
					SHOW_PROGRESS
					LOG print_download)
				endif()
				message(STATUS "check MD5 for download package...")
				FILE(MD5 "${HIPE_EXTERNAL}/download/${FILE_HIPE_EXTERNAL_DEPS}" _file_sha)
				if(NOT "${_file_sha}" STREQUAL "${DEPENDENCIES_SHA}")
					message(STATUS "LOG FAILURE ${print_download}")
					message(ERROR "Fail to download archive Checksum differ expected ${DEPENDENCIES_SHA} different of ${_file_sha}")
				
				else()
					message(STATUS "Download of dependencies OK")
					message(STATUS "Extract Archives to ${HIPE_EXTERNAL}. Please wait...")
					execute_process(
									COMMAND ${CMAKE_COMMAND} -E tar -xzf "${HIPE_EXTERNAL}/download/${FILE_HIPE_EXTERNAL_DEPS}"
									WORKING_DIRECTORY "${HIPE_EXTERNAL}/"
									)
					set(FORCE_USE_PREBUILT_DEPENDENCIES OFF CACHE BOOL "Download and use the prebuilt dependencies" FORCE)
				endif()
			endif()
		endif()
	endif()
endif()