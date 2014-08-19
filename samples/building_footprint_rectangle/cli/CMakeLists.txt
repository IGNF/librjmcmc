list(APPEND CMAKE_MODULE_PATH ${INSTALL_CMAKE_DIR})

find_package( TIFF   REQUIRED )

include_directories(../core)
include_directories(${rjmcmc_INCLUDE_DIRS})
include_directories(${TIFF_INCLUDE_DIR})

file( GLOB HPP *.h *.hpp )
aux_source_directory( ${CMAKE_CURRENT_SOURCE_DIR} CPP )

add_definitions( ${rjmcmc_DEFINITIONS} ${TIFF_DEFINITIONS})
add_executable( building_footprint_rectangle_cli ${CPP} ${HPP} )
target_link_libraries( building_footprint_rectangle_cli ${rjmcmc_LIBRARIES} ${TIFF_LIBRARIES})

