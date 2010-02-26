file( GLOB CORE_HEADER_FILES *.h *.hpp )
file( GLOB ENERGY_HEADER_FILES ${CMAKE_SOURCE_DIR}/lib/energy/*.h ${CMAKE_SOURCE_DIR}/lib/energy/*.hpp )

aux_source_directory( ${CMAKE_CURRENT_SOURCE_DIR} CORE_SRC )
aux_source_directory( ${CMAKE_SOURCE_DIR}/lib/energy ENERGY_SRC )

add_library(building_footprint_rectangle 
  ${CORE_HEADER_FILES}   ${CORE_SRC}
  ${ENERGY_HEADER_FILES} ${ENERGY_SRC}
)

set( BUILDING_FOOTPRINT_RECTANGLE_LIBRARIES ${Boost_LIBRARIES} ${GEOMETRY_LIBRARIES} building_footprint_rectangle PARENT_SCOPE )
