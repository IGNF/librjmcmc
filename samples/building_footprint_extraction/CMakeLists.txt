
# Project name
project( building_footprint_extraction )

option(BUILD_CLI "build building_footprint_extraction CLI sample" ON)
option(BUILD_GUI "build building_footprint_extraction GUI sample" ON)
option(USE_CGAL  "" ON)

set( ROOT_DIR ${CMAKE_CURRENT_SOURCE_DIR} )

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

if(USE_CGAL)
  # Find CGAL
  find_package( CGAL COMPONENTS Core )
  if( CGAL_FOUND )
    include( ${CGAL_USE_FILE} )
    add_definitions( -DUSE_CGAL=1 )
  endif( CGAL_FOUND )
endif(USE_CGAL)

if(BUILD_CLI OR BUILD_GUI)
  include_directories( ${CMAKE_SOURCE_DIR}/lib/libRJMCMC/ ${CMAKE_CURRENT_SOURCE_DIR} )
  file( GLOB ALL_CORE_HEADER_FILES ${ROOT_DIR}/core/*.h ${ROOT_DIR}/core/*.hpp )
  file( GLOB ALL_GEOMETRY_HEADER_FILES ${ROOT_DIR}/geometry/*.h ${ROOT_DIR}/geometry/*.hpp )
  aux_source_directory( ${ROOT_DIR}/core ALL_CORE_SRC )
  aux_source_directory( ${ROOT_DIR}/geometry ALL_GEOMETRY_SRC )
  set( SRC ${ALL_CORE_HEADER_FILES} ${ALL_CORE_SRC} ${ALL_GEOMETRY_HEADER_FILES} ${ALL_GEOMETRY_SRC} )
  add_library(building_footprint_extraction ${SRC})
  set( BUILDING_FOOTPRINT_EXTRACTION_LIBRARIES ${Boost_LIBRARIES} ${CGAL_LIBRARIES} building_footprint_extraction )
endif(BUILD_CLI OR BUILD_GUI)

if(BUILD_CLI)
  add_subdirectory(cli)
endif(BUILD_CLI)

if(BUILD_GUI)
  add_subdirectory(gui)
endif(BUILD_GUI)
