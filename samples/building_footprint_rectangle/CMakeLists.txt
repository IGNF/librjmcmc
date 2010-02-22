
# Project name
project( building_footprint_rectangle )

option(BUILD_CLI "build building_footprint_rectangle CLI sample" ON)
option(BUILD_GUI "build building_footprint_rectangle GUI sample" ON)
option(USE_CGAL  "Use CGAL library" OFF)

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
  include_directories( ${CMAKE_SOURCE_DIR}/lib ${CMAKE_CURRENT_SOURCE_DIR} )
  file( GLOB ALL_CORE_HEADER_FILES ${ROOT_DIR}/core/*.h ${ROOT_DIR}/core/*.hpp )
  file( GLOB ALL_GEOMETRY_HEADER_FILES ${CMAKE_SOURCE_DIR}/lib/geometry/*.h ${CMAKE_SOURCE_DIR}/lib/geometry/*.hpp )
  file( GLOB ALL_ENERGY_HEADER_FILES ${CMAKE_SOURCE_DIR}/lib/energy/*.h ${CMAKE_SOURCE_DIR}/lib/energy/*.hpp )
  aux_source_directory( ${ROOT_DIR}/core ALL_CORE_SRC )
  aux_source_directory( ${CMAKE_SOURCE_DIR}/lib/geometry ALL_GEOMETRY_SRC )
  aux_source_directory( ${CMAKE_SOURCE_DIR}/lib/energy ALL_ENERGY_SRC )
  set( SRC ${ALL_CORE_HEADER_FILES} ${ALL_CORE_SRC} ${ALL_GEOMETRY_HEADER_FILES} ${ALL_GEOMETRY_SRC}  ${ALL_ENERGY_HEADER_FILES} ${ALL_ENERGY_SRC} )
  add_library(building_footprint_rectangle ${SRC})
  set( BUILDING_FOOTPRINT_RECTANGLE_LIBRARIES ${Boost_LIBRARIES} ${CGAL_LIBRARIES} building_footprint_rectangle )
endif(BUILD_CLI OR BUILD_GUI)

if(BUILD_CLI)
  add_subdirectory(cli)
endif(BUILD_CLI)

if(BUILD_GUI)
  add_subdirectory(gui)
endif(BUILD_GUI)
