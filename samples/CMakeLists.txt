cmake_minimum_required(VERSION 2.6)

add_subdirectory(building_footprint_extraction)
add_subdirectory(global_reconstruction)
add_subdirectory(building_footprint_rectangle)

option(BUILD_FACADE_ELEMENTS "build facade_elements sample" OFF)
if(BUILD_FACADE_ELEMENTS)
        add_subdirectory(facade_elements_rectangle)
endif(BUILD_FACADE_ELEMENTS)
