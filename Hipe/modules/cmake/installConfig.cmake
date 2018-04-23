include(GetPrerequisites)


include("${PROJECT_SOURCE_DIR}/cmake/getDependencies.cmake")
if("${CMAKE_INSTALL_CONFIG_NAME}" STREQUAL "Debug") 
	set(BUILD_CONFIG "Debug")
else()
	set(BUILD_CONFIG "Release")
endif()

message(STATUS "Find shared library dependencies...")
hipe_find_dependencies(list_deps ${target_name} ${BUILD_CONFIG})
message(STATUS "All libraries deps found are : ${list_deps}")

foreach(filename ${list_deps})
	message(STATUS "Copy file ${filename}  ---> ${CMAKE_INSTALL_PREFIX}/3rdparty/bin/${BUILD_CONFIG}")
	FILE(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/${BUILD_CONFIG}" TYPE FILE FILES "${filename}" )
endforeach()


