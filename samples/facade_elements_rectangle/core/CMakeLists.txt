file( GLOB CORE_HEADER_FILES *.h *.hpp )
file( GLOB OWN_ENERGY_HEADER_FILES ../energy/*.h ../energy/*.hpp )

aux_source_directory( ${CMAKE_CURRENT_SOURCE_DIR} CORE_SRC )
aux_source_directory( ../energy OWN_ENERGY_SRC )

add_library(facade_elements_rectangle 
  ${CORE_HEADER_FILES}   ${CORE_SRC}
  ${OWN_ENERGY_HEADER_FILES} ${OWN_ENERGY_SRC}
)

set( FACADE_ELEMENTS_RECTANGLE_LIBRARIES ${Boost_LIBRARIES} ${GEOMETRY_LIBRARIES} facade_elements_rectangle PARENT_SCOPE )
