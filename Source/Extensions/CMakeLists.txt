add_library( PReloaded STATIC "" )
target_sources( PReloaded PRIVATE PReloaded.cpp PReloaded.h )

FOClassicExtension( SERVER PReloaded )

##
## Prettify IDE
##

source_group( " "         REGULAR_EXPRESSION "\.([Cc]|[Cc][Pp][Pp]|[Hh])$" )
source_group( "CMake"     REGULAR_EXPRESSION "[Cc][Mm][Aa][Kk][Ee]" )

get_property( targets DIRECTORY "${CMAKE_CURRENT_LIST_DIR}" PROPERTY BUILDSYSTEM_TARGETS )
foreach( target IN LISTS targets )
	set_property( TARGET ${target} PROPERTY FOLDER "Extensions" )
endforeach()
