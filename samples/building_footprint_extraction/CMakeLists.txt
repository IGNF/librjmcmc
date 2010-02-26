
# Project name
project( building_footprint_extraction )

option(BUILD_CLI "build building_footprint_extraction CLI sample" ON)
option(BUILD_GUI "build building_footprint_extraction GUI sample" ON)

set( ROOT_DIR ${CMAKE_CURRENT_SOURCE_DIR} )

if(BUILD_CLI OR BUILD_GUI)
  if(${GEOMETRY_USE_FILE})
    include(${GEOMETRY_USE_FILE})
  endif()
  add_definitions(${GEOMETRY_DEFINITIONS})
  include_directories( ${CMAKE_SOURCE_DIR}/lib ${CMAKE_CURRENT_SOURCE_DIR} )
  file( GLOB ALL_CORE_HEADER_FILES ${ROOT_DIR}/core/*.h ${ROOT_DIR}/core/*.hpp )
  file( GLOB ALL_ENERGY_HEADER_FILES ${CMAKE_SOURCE_DIR}/lib/energy/*.h ${CMAKE_SOURCE_DIR}/lib/energy/*.hpp )
  aux_source_directory( ${ROOT_DIR}/core ALL_CORE_SRC )
  aux_source_directory( ${CMAKE_SOURCE_DIR}/lib/energy ALL_ENERGY_SRC )
  set( SRC ${ALL_CORE_HEADER_FILES} ${ALL_CORE_SRC} ${ALL_ENERGY_HEADER_FILES} ${ALL_ENERGY_SRC} )
  add_library(building_footprint_extraction ${SRC})
  set( BUILDING_FOOTPRINT_EXTRACTION_LIBRARIES ${Boost_LIBRARIES} ${GEOMETRY_LIBRARIES} building_footprint_extraction )
endif(BUILD_CLI OR BUILD_GUI)

if(BUILD_CLI)
  add_subdirectory(cli)
endif(BUILD_CLI)

if(BUILD_GUI)
  add_subdirectory(gui)
endif(BUILD_GUI)
