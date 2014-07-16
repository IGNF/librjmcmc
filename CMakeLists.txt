#########################################################
#                 general setup                       ###
#########################################################
cmake_minimum_required(VERSION 2.6)
project(rjmcmc CXX)

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin)
set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_BUILD_TYPE "Release")
set(ROOT_DIR ${PROJECT_SOURCE_DIR})


#########################################################
#            platform-specific setup                  ###
#########################################################
set(rjmcmc_DEFINITIONS "")
if(UNIX)
        set(rjmcmc_DEFINITIONS "${rjmcmc_DEFINITIONS} -fPIC")
        set(rjmcmc_DEFINITIONS "${rjmcmc_DEFINITIONS} -Wno-deprecated") # hide BGL hash_set deprecation warning
endif(UNIX)

if(WIN32)
        set(rjmcmc_DEFINITIONS "${rjmcmc_DEFINITIONS} /MP")
endif(WIN32)


#########################################################
#            Compilation OPTION                       ###
#########################################################

option(BUILD_SHARED_LIBS "Dynamic libraries" ON)
option(BUILD_SHARED_EXE  "Dynamic executables" ON)

#########################################################
#            C++11 support OPTION                     ###
#########################################################
option(USE_CPP11 "Enable C++11 compilation" ON)
if(USE_CPP11)
    set(rjmcmc_DEFINITIONS "${rjmcmc_DEFINITIONS} -DUSE_CPP11=1 -std=c++0x")
endif(USE_CPP11)

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

#########################################################
#            Finding REQUIRED package Boost           ###
#########################################################
set(Boost_ADDITIONAL_VERSIONS 
		   "1.40.0" "1.40" 
		   "1.41.0" "1.41" 
		   "1.42.0" "1.42" 
		   "1.43.0" "1.43" 
		   "1.44.0" "1.44" 
		   "1.45" "1.45.0" 
		   "1.46" "1.46.1"
		    "1.47" "1.47.1"
		    "1.48" "1.48.1")
set(Boost_USE_STATIC_LIBS OFF CACHE BOOL "use boost static lib")
set(Boost_USE_MULTITHREAD ON CACHE BOOL "use boost multi thread lib")
set(BOOST_ROOT "" CACHE PATH "path to boost root directory")
find_package( Boost 1.37 COMPONENTS thread program_options system filesystem REQUIRED)
include_directories( ${Boost_INCLUDE_DIRS} )
link_directories( ${Boost_LIBRARY_DIRS} )

if(WIN32)
    message(STATUS "Boost_LIB_DIAGNOSTIC_DEFINITIONS ${Boost_LIB_DIAGNOSTIC_DEFINITIONS} ")
    set(rjmcmc_DEFINITIONS "${rjmcmc_DEFINITIONS} ${Boost_LIB_DIAGNOSTIC_DEFINITIONS}" )
    if(NOT Boost_USE_STATIC_LIBS)
        set(rjmcmc_DEFINITIONS "${rjmcmc_DEFINITIONS} /DBOOST_ALL_DYN_LINK" )
    endif(NOT Boost_USE_STATIC_LIBS)
endif(WIN32)

#########################################################
#            Finding OPTIONAL package CGAL            ###
#########################################################
option(USE_CGAL  "Use CGAL library" OFF)
if(USE_CGAL)
  find_package( CGAL COMPONENTS Core )
  set(USE_CGAL ${CGAL_FOUND})
endif(USE_CGAL)

if(USE_CGAL)
  include(${CGAL_USE_FILE})
  set(rjmcmc_DEFINITIONS "${rjmcmc_DEFINITIONS} -DUSE_CGAL=1")
  set(GEOMETRY_LIBRARIES ${CGAL_LIBRARIES})
else(USE_CGAL)
  set(GEOMETRY_LIBRARIES )
endif(USE_CGAL)


#########################################################
#            Recursion to subdirs                     ###
#########################################################

set(rjmcmc_INCLUDE_DIRS "${Boost_INCLUDE_DIRS}")
set(rjmcmc_LIBRARIES "${GEOMETRY_LIBRARIES}")

add_definitions(${rjmcmc_DEFINITIONS})

add_subdirectory(extern)
add_subdirectory(include)
add_subdirectory(src)
add_subdirectory(samples)
add_subdirectory(doc)
