cmake_minimum_required( VERSION 3.12 FATAL_ERROR )

find_package( Git REQUIRED QUIET )

execute_process(
	COMMAND ${GIT_EXECUTABLE} config -f .gitmodules --get submodule.Engine.path
	WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}
	OUTPUT_VARIABLE ENGINE_DIR
	OUTPUT_STRIP_TRAILING_WHITESPACE
)

if( NOT ENGINE_DIR )
	message( FATAL_ERROR "Cannot find git submodule 'Engine' path" )
endif()

list( APPEND CMAKE_MODULE_PATH "${ENGINE_DIR}/CMake" )
include( AutomatedBuild )

set( BUILD_DIR "Build" )
if( UNIX )
	set( BUILD_FILE      "Makefile" )
	set( BUILD_GENERATOR "Unix Makefiles" )
	set( BUILD_TOOL      "Linux32" )
elseif( WIN32 )
	set( BUILD_FILE      "PReloaded.sln" )
	if( DEFINED ENV{APPVEYOR_BUILD_WORKER_IMAGE} )
		set( BUILD_GENERATOR "Visual Studio 10 2010" )
	else()
		set( BUILD_GENERATOR "Visual Studio 15 2017" )
		set( BUILD_TOOL      "v100" )
	endif()
endif()

CreateBuildDirectory( "${BUILD_DIR}" "${BUILD_GENERATOR}" "${BUILD_TOOL}" "${BUILD_FILE}" )
if( BUILD_FAIL )
	message( FATAL_ERROR "Build error" )
endif()

RunAllBuilds()
if( BUILD_FAIL )
	message( FATAL_ERROR "Build error" )
endif()
