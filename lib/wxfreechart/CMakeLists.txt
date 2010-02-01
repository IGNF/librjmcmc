cmake_minimum_required(VERSION 2.6)

FIND_PACKAGE(wxWidgets)
IF(wxWidgets_FOUND)  
	INCLUDE(${wxWidgets_USE_FILE})
	
	include_directories( include )
	file( GLOB wxfreechart_HEADER_FILES include/wx/*.h include/wx/*/*.h )
	add_subdirectory(sample)
	add_subdirectory(src)
ELSE()
	MESSAGE("wxWidgets not found ! Please set wxWidgets path to use wxfreechart...")
ENDIF()
