find_package( TIFF REQUIRED )

add_executable( building_footprint_extraction_cli ${SRC} ${CLI_SRC} )
target_link_libraries( building_footprint_extraction_cli ${BUILDING_FOOTPRINT_EXTRACTION_LIBRARIES} ${TIFF_LIBRARIES} )
