cmake_minimum_required(VERSION 2.8)
project( building_footprint_rectangle )

find_package(rjmcmc QUIET COMPONENTS wx)

option(BUILD_BUILDING_FOOTPRINT_RECTANGLE_CLI "build building_footprint_rectangle CLI sample" ON)
option(BUILD_BUILDING_FOOTPRINT_RECTANGLE_GUI "build building_footprint_rectangle GUI sample" ${rjmcmc-wx_FOUND})
option(BUILD_BUILDING_FOOTPRINT_RECTANGLE_GILVIEWER "build building_footprint_rectangle GILVIEWER plugin" ${rjmcmc-wx_FOUND})


#copy the data image
add_custom_target(building_footprint_rectangle_data
  "${CMAKE_COMMAND}" -E make_directory "${CMAKE_BINARY_DIR}/data"
  COMMAND "${CMAKE_COMMAND}" -E copy_if_different "${CMAKE_CURRENT_SOURCE_DIR}/../data/ZTerrain_c3.tif" "${CMAKE_BINARY_DIR}/data"
)

if(BUILD_BUILDING_FOOTPRINT_RECTANGLE_CLI)
  add_subdirectory(cli)
  add_dependencies(building_footprint_rectangle_cli building_footprint_rectangle_data)
endif()

if(BUILD_BUILDING_FOOTPRINT_RECTANGLE_GUI AND rjmcmc-wx_FOUND)
  add_subdirectory(gui)
  add_dependencies(building_footprint_rectangle_gui building_footprint_rectangle_data)
endif()


if(BUILD_BUILDING_FOOTPRINT_RECTANGLE_GILVIEWER AND rjmcmc-wx_FOUND)
  add_subdirectory(gilviewer)
  add_dependencies(GilViewer-building_footprint_rectangle building_footprint_rectangle_data)
endif()
