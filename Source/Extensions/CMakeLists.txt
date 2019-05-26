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
## AS
##

foreach( target IN ITEMS Client Mapper Server )
	add_library( AS.${target} STATIC )
	target_sources( AS.${target}
		PRIVATE
			${CMAKE_CURRENT_LIST_FILE}
			AS/AS.cpp
			AS/AS.h
	)
	set_property( TARGET AS.${target} PROPERTY FOLDER "Extensions/AS" )
	FOClassicExtension( ${target} AS.${target} )
endforeach()

##
## BufferLazy
##

foreach( target IN ITEMS Client Server )
	add_library( BufferLazy.${target} STATIC )
	target_sources( BufferLazy.${target}
		PRIVATE
			${CMAKE_CURRENT_LIST_FILE}
			BufferLazy/BufferLazy.cpp
			BufferLazy/BufferLazy.h
	)
	set_property( TARGET BufferLazy.${target} PROPERTY FOLDER "Extensions/BufferLazy" )
	FOClassicExtension( ${target} BufferLazy.${target} )
endforeach()

##
## CheckLook
##

add_library( CheckLook STATIC )
target_sources( CheckLook
	PRIVATE
		CheckLook/CheckLook.cpp
		CheckLook/CheckLook.h
		CheckLook/Move.h
		CheckLook/Lookup.cpp
		CheckLook/WallDist.cpp
		CheckLook/WallDist.h
)
FOClassicExtension( SERVER CheckLook )

##
## MapperUtils
##

add_library( MapperUtils STATIC )
target_sources( MapperUtils
	PRIVATE
		MapperUtils/MapperUtils.cpp
		MapperUtils/MapperUtils.h
)
FOClassicExtension( MAPPER MapperUtils )

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
## Utils
##

add_library( Utils STATIC )
target_sources( Utils
	PRIVATE
		${CMAKE_CURRENT_LIST_FILE}
		Utils/ExtEvents.cpp
		Utils/ExtEvents.h
		Utils/ProtoMapEx.cpp
		Utils/ProtoMapEx.h
		Utils/PUtils.cpp
		Utils/PUtils.h
)
FOClassicExtension( SERVER Utils )

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
