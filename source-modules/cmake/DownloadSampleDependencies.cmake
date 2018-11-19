
SET(SAMPLE_DIR ${CMAKE_INSTALL_PREFIX}/../hipe-core/workingdir/root/)

SET(FILE_SAMPLE_DEPS "Data-light.zip")
set(DEPENDENCIES_SHA "b18ce95e37e23ff33e02b8e202274b22")
if (${BUILD_SAMPLE})
	message(STATUS "Download Sample dependencies...")
	if (NOT EXISTS "${SAMPLE_DIR}/Data-light" OR ${DOWNLOAD_SAMPLE})
		file(MAKE_DIRECTORY "${SAMPLE_DIR}")
		
		file(MAKE_DIRECTORY "${SAMPLE_DIR}/download")
		SET(_file_download_md5 "0")
		if (EXISTS "${SAMPLE_DIR}/download/${FILE_SAMPLE_DEPS}")
			FILE(MD5 "${SAMPLE_DIR}/download/${FILE_SAMPLE_DEPS}" _file_download_md5)
		endif()
		if(NOT "${_file_download_md5}" STREQUAL "${DEPENDENCIES_SHA}")
			message(STATUS "Need to download Samples dependencies...")
			FILE(DOWNLOAD "ftp://dupihome.ddns.net/Samples/${FILE_SAMPLE_DEPS}" "${SAMPLE_DIR}/download/${FILE_SAMPLE_DEPS}"
			#FILE(DOWNLOAD "ftp://192.168.1.107/HipeExternal/${FILE_SAMPLE_DEPS}" "${SAMPLE_DIR}/download/${FILE_SAMPLE_DEPS}"
			USERPWD "public:guest01"
			#EXPECTED_MD5 ${DEPENDENCIES_SHA}
			SHOW_PROGRESS
			LOG print_download)
		endif()
		# message(STATUS "check MD5 for download package...")
		# FILE(MD5 "${SAMPLE_DIR}/download/${FILE_SAMPLE_DEPS}" _file_sha)
		# if(NOT "${_file_sha}" STREQUAL "${DEPENDENCIES_SHA}")
			message(STATUS "LOG Download ${print_download}")
			# message(ERROR "Fail to download archive Checksum differ expected ${DEPENDENCIES_SHA} different of ${_file_sha}")
		
		# else()
			message(STATUS "Download of dependencies OK")
			message(STATUS "Extract Archives to ${SAMPLE_DIR}. Please wait...")
			execute_process(
							COMMAND ${CMAKE_COMMAND} -E tar -xzf "${SAMPLE_DIR}/download/${FILE_SAMPLE_DEPS}"
							WORKING_DIRECTORY "${SAMPLE_DIR}/"
							)
			set(FORCE_USE_PREBUILT_DEPENDENCIES OFF CACHE BOOL "Download and use the prebuilt dependencies" FORCE)
		# endif()
	endif()
endif()
