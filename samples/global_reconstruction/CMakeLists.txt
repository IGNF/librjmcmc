
# Project name
project( global_reconstruction )

option(BUILD_GLOBAL_RECONSTRUCTION "build global_reconstruction sample" ON)

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

find_package( TIFF REQUIRED )

if(BUILD_GLOBAL_RECONSTRUCTION)
  file( GLOB ALL_HEADER_FILES *.h *.hpp )
  aux_source_directory( ${ROOT_DIR} ALL_SRC )
  set( SRC  ${ALL_HEADER_FILES} ${ALL_SRC} )

  add_executable( global_reconstruction ${SRC} ${ALL_LIBRJMCMC_HEADERS_FILES} )
  target_link_libraries(global_reconstruction ${TIFF_LIBRARIES} )
endif(BUILD_GLOBAL_RECONSTRUCTION)

