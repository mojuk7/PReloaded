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
## See CMakeFOClassic.txt for extensions setup
##

message( STATUS "Configuring engine..." )

# handled by engine config
# (IDE only) all engine projects are inside folder specified
set( FOCLASSIC_FOLDER_PREFIX "Engine")

# handled by engine config
# copies executables to specific directories after compilation
list( APPEND FOCLASSIC_COPY_CLIENT_DIR     "${P_CLIENT_DIR}" )
list( APPEND FOCLASSIC_COPY_MAPPER_DIR     "${P_MAPPER_DIR}" )
list( APPEND FOCLASSIC_COPY_SERVER_DIR     "${P_SERVER_DIR}" )
list( APPEND FOCLASSIC_COPY_ASCOMPILER_DIR "${P_SERVER_DIR}/scripts" )

add_subdirectory( "${P_ROOT_DIR}/Source/Engine" )
message( STATUS "Configuring engine... OK" )

##
## Custom target for accessing CMakeFOClassic.txt from IDE
##

add_custom_target( CMakeFOClassic SOURCES "${P_ROOT_DIR}/CMakeFOClassic.txt" )
set_property( TARGET CMakeFOClassic PROPERTY FOLDER "Extensions")

##
## Prettify IDE
##

set_property( GLOBAL PROPERTY USE_FOLDERS ON )
set_property( GLOBAL PROPERTY PREDEFINED_TARGETS_FOLDER "CMake" )
