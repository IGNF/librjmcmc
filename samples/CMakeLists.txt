cmake_minimum_required(VERSION 2.6)

file( GLOB ENERGY_HEADER_FILES ../lib/energy/*.h ../lib/energy/*.hpp )
file( GLOB PARAM_HEADER_FILES ../lib/param/*.h ../lib/param/*.hpp )
file( GLOB_RECURSE RJMCMC_HEADER_FILES ../lib/rjmcmc/*.h ../lib/rjmcmc/*.hpp )
file( GLOB IMAGE_HEADER_FILES ../lib/image/*.h ../lib/image/*.hpp )
set( ALL_LIBRJMCMC_HEADERS_FILES ${ENERGY_HEADER_FILES} ${PARAM_HEADER_FILES} ${RJMCMC_HEADER_FILES} ${IMAGE_HEADER_FILES} )

add_subdirectory(building_footprint_extraction)
add_subdirectory(global_reconstruction)
add_subdirectory(building_footprint_rectangle)

option(BUILD_FACADE_ELEMENTS "build facade_elements sample" OFF)
if(BUILD_FACADE_ELEMENTS)
        add_subdirectory(facade_elements_rectangle)
endif(BUILD_FACADE_ELEMENTS)
