cmake_minimum_required(VERSION 2.8)

file( GLOB HPP *.h *.hpp )
aux_source_directory( ${CMAKE_CURRENT_SOURCE_DIR} CPP )

include_directories( ${CMAKE_SOURCE_DIR}/include )

add_executable( rejection_variate ${HPP} ${CPP}  ${rjmcmc_HPP} )
