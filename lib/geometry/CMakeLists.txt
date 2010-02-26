cmake_minimum_required(VERSION 2.6)

if(NOT USE_CGAL)
  file( GLOB GEOMETRY_HEADER_FILES *.h *.hpp )
  aux_source_directory( . GEOMETRY_SRC )
  add_library(geometry STATIC ${GEOMETRY_HEADER_FILES} ${GEOMETRY_SRC})
endif(NOT USE_CGAL)
