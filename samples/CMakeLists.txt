cmake_minimum_required(VERSION 2.6)

file( GLOB ALL_LIBRJMCMC_HEADERS_FILES
    ../lib/*.h ../lib/*.hpp
    ../lib/*/*.h ../lib/*/*.hpp
    ../lib/*/*/*.h ../lib/*/*/*.hpp
    ../lib/*/*/*/*.h ../lib/*/*/*/*.hpp
)

add_subdirectory(building_footprint_extraction)
add_subdirectory(building_footprint_rectangle)
add_subdirectory(rejection_variate)

