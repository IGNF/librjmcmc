cmake_minimum_required(VERSION 2.6)

file( GLOB DOC_FILES src/* )
file( GLOB DOXYGEN_FILES doxygen/* )
file( GLOB BJAM_FILES Jamroot )

add_executable(doc ${DOC_FILES} ${BJAM_FILES} ${DOXYGEN_FILES})
