cmake_minimum_required(VERSION 2.6)

# Project name
project( libMPP CXX)

set(PACKAGE "libMPP")
set(PACKAGE_BUGREPORT "bredif tournaire")
set(PACKAGE_NAME "libMPP")
set(PACKAGE_STRING " RJMCMC library ")
set(PACKAGE_TARNAME "libmpp")
set(PACKAGE_VERSION "1.0")
set(VERSION "1.0")

set( ROOT_DIR ${PROJECT_SOURCE_DIR})
# SET classical path for the MATIS library

#########################################################
# INSTALL DEFAULT CONFIGURATION and CMAKE_PREFIX_PATH ###
#########################################################
#first set the correct path for install
if(UNIX)
       set(INSTALL_PREFIX "/usr/local" CACHE PATH " install path" )
       set(CMAKE_PREFIX_PATH ${INSTALL_PREFIX} ${CMAKE_PREFIX_PATH}  )
endif(UNIX)
if(WIN32)
       set(INSTALL_PREFIX "C:/Program Files/MATIS" CACHE PATH " install path")
       set(CMAKE_PREFIX_PATH ${INSTALL_PREFIX} ${CMAKE_INSTALL_PREFIX} )
endif(WIN32)
set(CMAKE_INSTALL_PREFIX ${INSTALL_PREFIX} CACHE INTERNAL  " real install path" FORCE)


include(libMPP-doc.cmake)

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin)
set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR}/lib)

option(BUILD_SHARED_LIBS "Dynamic libraries" ON)
option(BUILD_SHARED_EXE  "Dynamic executables" ON)

add_subdirectory(samples)
add_subdirectory(lib)
