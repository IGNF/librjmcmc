#file( GLOB CORE_HEADER_FILES *.h *.hpp )
#aux_source_directory( ${CMAKE_CURRENT_SOURCE_DIR} CORE_SRC )
#add_library(building_footprint_extraction ${CORE_HEADER_FILES}   ${CORE_SRC})

# set( BUILDING_FOOTPRINT_EXTRACTION_LIBRARIES ${Boost_LIBRARIES} ${GEOMETRY_LIBRARIES} building_footprint_extraction PARENT_SCOPE )


set( BUILDING_FOOTPRINT_EXTRACTION_LIBRARIES ${Boost_LIBRARIES} ${GEOMETRY_LIBRARIES} PARENT_SCOPE )

#copy the data image
add_custom_command(TARGET building_footprint_extraction
    PRE_BUILD
    COMMAND "${CMAKE_COMMAND}" ARGS -E copy_if_different "${CMAKE_SOURCE_DIR}/data/test.tif" "${CMAKE_SOURCE_DIR}/data/test_ndvi.tif" "${CMAKE_BINARY_DIR}/data"
)
