cmake_minimum_required(VERSION 3.12.2)

##
## Base config
##

project( PReloaded LANGUAGES CXX )

message( STATUS "" )
message( STATUS "${PROJECT_NAME}" )
message( STATUS "" )

set( CMAKE_CXX_STANDARD       11 )
set( CMAKE_SKIP_INSTALL_RULES TRUE )
set( CMAKE_VERBOSE_MAKEFILE   TRUE )

##
## Main directories
##

set( P_ROOT_DIR   "${CMAKE_CURRENT_LIST_DIR}" )
set( P_CLIENT_DIR "${P_ROOT_DIR}/Client" )
set( P_MAPPER_DIR "${P_ROOT_DIR}/Mapper" )
set( P_SERVER_DIR "${P_ROOT_DIR}/Server" )

##
## Engine configuration
## See CMakeFOClassic.txt for additional setup
##

message( STATUS "Configuring engine..." )
set( ENGINE_DIR              "${P_ROOT_DIR}/Source/Engine" )
set( FOCLASSIC_FOLDER_PREFIX "Engine")
add_subdirectory( ${ENGINE_DIR} )
message( STATUS "Configuring engine... OK" )

##
## Prettify IDE
##

set_property( GLOBAL PROPERTY USE_FOLDERS ON )
set_property( GLOBAL PROPERTY PREDEFINED_TARGETS_FOLDER "CMake" )
set_property( TARGET ${FOCLASSIC_ALL_TARGETS} PROPERTY FOLDER Engine )
