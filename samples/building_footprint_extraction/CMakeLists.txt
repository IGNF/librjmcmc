cmake_minimum_required(VERSION 2.8)
# Project name
project( building_footprint_extraction )

include_directories( ${CMAKE_CURRENT_SOURCE_DIR} )

option(BUILD_BUILDING_FOOTPRINT_EXTRACTION_CLI "build building_footprint_extraction CLI sample" OFF)
option(BUILD_BUILDING_FOOTPRINT_EXTRACTION_GUI "build building_footprint_extraction GUI sample" OFF)

#copy the data image
add_custom_target(building_footprint_extraction_data
  "${CMAKE_COMMAND}" -E make_directory "${CMAKE_BINARY_DIR}/data"
  COMMAND "${CMAKE_COMMAND}" -E copy_if_different "${CMAKE_CURRENT_SOURCE_DIR}/../data/test.tif"      "${CMAKE_BINARY_DIR}/data"
  COMMAND "${CMAKE_COMMAND}" -E copy_if_different "${CMAKE_CURRENT_SOURCE_DIR}/../data/test_ndvi.tif" "${CMAKE_BINARY_DIR}/data"
)

if(BUILD_BUILDING_FOOTPRINT_EXTRACTION_CLI OR BUILD_BUILDING_FOOTPRINT_EXTRACTION_GUI)
  add_subdirectory(core)
endif(BUILD_BUILDING_FOOTPRINT_EXTRACTION_CLI OR BUILD_BUILDING_FOOTPRINT_EXTRACTION_GUI)

if(BUILD_BUILDING_FOOTPRINT_EXTRACTION_CLI)
  add_subdirectory(cli)
  add_dependencies(building_footprint_extraction_cli building_footprint_extraction_data)
endif(BUILD_BUILDING_FOOTPRINT_EXTRACTION_CLI)

if(BUILD_BUILDING_FOOTPRINT_EXTRACTION_GUI)
  add_subdirectory(gui)
  add_dependencies(building_footprint_extraction_gui building_footprint_extraction_data)
endif(BUILD_BUILDING_FOOTPRINT_EXTRACTION_GUI)
