include(GetPrerequisites)

message(STATUS "Install directories : ${PROJECT_SOURCE_DIR}")
file(GLOB_RECURSE source_header_files
	LIST_DIRECTORIES false
	RELATIVE ${PROJECT_SOURCE_DIR}
	"${PROJECT_SOURCE_DIR}/*.h"
	"${PROJECT_SOURCE_DIR}/*.hpp"
	"${PROJECT_SOURCE_DIR}/*.cpp")
	
file(GLOB_RECURSE source_header_files_local
	LIST_DIRECTORIES false
	"*.h"
	"*.hpp"
	"*.cpp")

	
message(STATUS "Find all source files : ${source_header_files}")
message(STATUS "Read file license :${LICENSE_DIR}/../LICENCE_AGPL_short.txt")

file(READ "${LICENSE_DIR}/../LICENCE_AGPL_short.txt" HIPE_LICENSE_FILE)
SET(HIPE_LICENSE "READ LICENSE BEFORE ANY USAGE\r\n${HIPE_LICENSE_FILE}")
message(STATUS "Add header license to Header license : ${HIPE_LICENSE}")

foreach(filename ${source_header_files})
	message(STATUS "Add header license : ${PROJECT_SOURCE_DIR}/${filename}")
	CONFIGURE_FILE("${PROJECT_SOURCE_DIR}/${filename}" "${PROJECT_SOURCE_DIR}/${filename}" @ONLY)
endforeach()


foreach(filename ${source_header_files_local})
	message(STATUS "Add header license : ${filename}")
	CONFIGURE_FILE("${filename}" "${filename}" @ONLY)
endforeach()