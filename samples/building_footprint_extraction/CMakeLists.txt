cmake_minimum_required(VERSION 2.6)

# Project name
project( building_footprint_extraction )

set( ROOT_DIR ${CMAKE_SOURCE_DIR} )

file( GLOB ALL_CORE_HEADER_FILES ${ROOT_DIR}/core/*.h ${ROOT_DIR}/core/*.hpp )
file( GLOB ALL_GEOMETRY_HEADER_FILES ${ROOT_DIR}/geometry/*.h ${ROOT_DIR}/geometry/*.hpp )
file( GLOB ALL_GUI_HEADER_FILES ${ROOT_DIR}/gui/*.h ${ROOT_DIR}/gui/*.hpp )
file( GLOB ALL_CLI_HEADER_FILES ${ROOT_DIR}/cli/*.h ${ROOT_DIR}/cli/*.hpp )
file( GLOB ALL_MAIN_HEADER_FILES ${ROOT_DIR}/*.h ${ROOT_DIR}/*.hpp )
aux_source_directory( ${ROOT_DIR}/core ALL_CORE_SRC )
aux_source_directory( ${ROOT_DIR}/geometry ALL_GEOMETRY_SRC )
aux_source_directory( ${ROOT_DIR}/gui ALL_GUI_SRC )
aux_source_directory( ${ROOT_DIR}/cli ALL_CLI_SRC )
aux_source_directory( ${ROOT_DIR} ALL_MAIN_SRC )
set( SRC ${ALL_CORE_HEADER_FILES} ${ALL_CORE_SRC}
         ${ALL_GEOMETRY_HEADER_FILES} ${ALL_GEOMETRY_SRC}
         ${ALL_MAIN_HEADER_FILES} ${ALL_MAIN_SRC} )
set( CLI_SRC ${ALL_CLI_HEADER_FILES} ${ALL_CLI_SRC} )
set( GUI_SRC ${ALL_GUI_HEADER_FILES} ${ALL_GUI_SRC} ) 

set( RJMCMC_INCLUDE_DIR ../../lib/libRJMCMC/ ../../extern/ )

# Find BOOST
find_package( Boost 1.37 COMPONENTS thread program_options system )
set( Boost_ADDITIONAL_VERSIONS "1.39.0" "1.39" "1.40.0" "1.40" )
find_package( Boost 1.37 COMPONENTS thread program_options system )
if( NOT Boost_FOUND )
	message(FATAL_ERROR "Boost not found ! Please set Boost path ...")
else()
	include_directories( ${Boost_INCLUDE_DIRS} )
	link_directories( ${Boost_LIBRARY_DIRS} )
endif()

# Find CGAL
find_package( CGAL COMPONENTS Core )
if( CGAL_FOUND )
	include( ${CGAL_USE_FILE} )
endif()

set( BUILDING_FOOTPRINT_EXTRACTION_INCLUDE_DIR . )
include_directories( ${RJMCMC_INCLUDE_DIR} ${BUILDING_FOOTPRINT_EXTRACTION_INCLUDE_DIR} )
set( BUILDING_FOOTPRINT_EXTRACTION_LIBRARIES ${Boost_LIBRARIES} ${CGAL_LIBRARIES} )

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin)
set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR}/lib)

add_subdirectory(cli)
add_subdirectory(gui)
