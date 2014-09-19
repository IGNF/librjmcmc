cmake_minimum_required(VERSION 2.8)
find_package( rjmcmc REQUIRED )

include_directories(${rjmcmc_INCLUDE_DIRS})
add_definitions( ${rjmcmc_DEFINITIONS})

add_executable( rejection_variate rejection_variate.cpp )
add_executable( raster_variate raster_variate.cpp )

