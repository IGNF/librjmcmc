# Project name
project( building_footprint_rectangle )

include_directories( ${CMAKE_CURRENT_SOURCE_DIR} )

option(BUILD_BUILDING_FOOTPRINT_RECTANGLE_CLI "build building_footprint_rectangle CLI sample" ON)
option(BUILD_BUILDING_FOOTPRINT_RECTANGLE_GUI "build building_footprint_rectangle GUI sample" ON)
option(BUILD_BUILDING_FOOTPRINT_RECTANGLE_GILVIEWER "build building_footprint_rectangle GILVIEWER plugin" ON)


#copy the data image
add_custom_target(building_footprint_rectangle_data
  "${CMAKE_COMMAND}" -E make_directory "${CMAKE_BINARY_DIR}/data"
  COMMAND "${CMAKE_COMMAND}" -E copy_if_different "${CMAKE_SOURCE_DIR}/data/ZTerrain_c3.tif" "${CMAKE_BINARY_DIR}/data"
)

add_subdirectory(core)

if(BUILD_BUILDING_FOOTPRINT_RECTANGLE_CLI)
  add_subdirectory(cli)
  add_dependencies(building_footprint_rectangle_cli building_footprint_rectangle_data)
endif(BUILD_BUILDING_FOOTPRINT_RECTANGLE_CLI)

if(BUILD_BUILDING_FOOTPRINT_RECTANGLE_GUI)
  add_subdirectory(gui)
  add_dependencies(building_footprint_rectangle_gui building_footprint_rectangle_data)
endif(BUILD_BUILDING_FOOTPRINT_RECTANGLE_GUI)


if(BUILD_BUILDING_FOOTPRINT_RECTANGLE_GILVIEWER)
  add_subdirectory(gilviewer)
  add_dependencies(GilViewer-building_footprint_rectangle building_footprint_rectangle_data)
endif(BUILD_BUILDING_FOOTPRINT_RECTANGLE_GILVIEWER)
