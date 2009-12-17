#include doxygen macro
include(MatisDoxygenMacros)
set(CMAKE_VERBOSITY 1)

#config documentation
OPTION(CUSTOM_BUILD "if set, you could access documentation options " OFF)
__config_documentation()

#set up src using by doxygen documentation
SET(DOXY_EXTRACT_ALL "YES")
SET(DOXY_COLS_IN_ALPHA_INDEX "2")
	
set( building_footprint_DOC_SRC	${ALL_CORE_HEADER_FILES} 
    						    ${ALL_GEOMETRY_HEADER_FILES} 
         						${ALL_MAIN_HEADER_FILES} 
								${ALL_CLI_HEADER_FILES} 
								${ALL_GUI_HEADER_FILES} 
	)

__add_documentation(building_footprint building_footprint_DOC_SRC)
