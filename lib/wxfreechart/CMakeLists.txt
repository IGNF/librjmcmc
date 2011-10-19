cmake_minimum_required(VERSION 2.6)

FIND_PACKAGE(wxWidgets REQUIRED)
INCLUDE(${wxWidgets_USE_FILE})
	
include_directories( include )
file( GLOB WXFREECHART_HEADERS include/wx/*.h include/wx/*/*.h )
add_subdirectory(sample)
add_subdirectory(src)
