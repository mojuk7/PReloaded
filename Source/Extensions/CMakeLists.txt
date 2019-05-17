set( P_CLIENT_MAPPER_SERVER_SOURCES
	PReloaded.cpp
	PReloaded.h
)

##
## PReloaded
##

foreach( target IN ITEMS Client Mapper Server )
	add_library( PReloaded.${target} STATIC )
	target_sources( PReloaded.${target}
		PRIVATE
			${CMAKE_CURRENT_LIST_FILE}
			${P_CLIENT_MAPPER_SERVER_SOURCES}
	)
	set_property( TARGET PReloaded.${target} PROPERTY FOLDER "Extensions/PReloaded" )
	FOClassicExtension( ${target} PReloaded.${target} )
endforeach()

##
## Parameters
##

foreach( target IN ITEMS Client Server )
	add_library( Parameters.${target} STATIC )
	target_sources( Parameters.${target}
		PRIVATE
			${CMAKE_CURRENT_LIST_FILE}
			Parameters/Parameters.cpp
			Parameters/Parameters.h
	)

	set_property( TARGET Parameters.${target} PROPERTY FOLDER "Extensions/Parameters" )
	FOClassicExtension( ${target} Parameters.${target} )
endforeach()

##
## Prettify IDE
##

source_group( " "         REGULAR_EXPRESSION "\.([Cc]|[Cc][Pp][Pp]|[Hh])$" )
source_group( "CMake"     REGULAR_EXPRESSION "[Cc][Mm][Aa][Kk][Ee]" )

get_property( targets DIRECTORY "${CMAKE_CURRENT_LIST_DIR}" PROPERTY BUILDSYSTEM_TARGETS )
foreach( target IN LISTS targets )
	# add Source/Extensions/ to include directories
	target_include_directories( ${target} PRIVATE "${CMAKE_CURRENT_LIST_DIR}" )

	# add Server/scripts/ to include directories
	# mostly for _defines.fos
	target_include_directories( ${target} PRIVATE "${P_SERVER_DIR}/scripts" )

	# add default FOLDER property to targets which don't have it set yet
	get_property( already_set TARGET ${target} PROPERTY FOLDER SET )
	if( NOT already_set )
		set_property( TARGET ${target} PROPERTY FOLDER "Extensions" )
	endif()
endforeach()
