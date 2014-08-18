cmake_minimum_required(VERSION 2.6)
find_package( rjmcmc REQUIRED )

include_directories(${rjmcmc_INCLUDE_DIRS})
add_definitions( ${rjmcmc_DEFINITIONS})

add_executable( quickstart quickstart.cpp )
target_link_libraries( quickstart ${rjmcmc_LIBRARIES})

