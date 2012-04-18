#########################################################
#                 general setup                       ###
#########################################################
cmake_minimum_required(VERSION 2.6)
project( librjmcmc CXX)

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin)
set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_BUILD_TYPE "Release")
set(ROOT_DIR ${PROJECT_SOURCE_DIR})

include_directories( lib )

#########################################################
#            platform-specific setup                  ###
#########################################################
if(UNIX)
        add_definitions(-fPIC)
        add_definitions(-Wno-deprecated) # hide BGL hash_set deprecation warning
        set(wxWidgets_REQUIRED_COMPONENTS adv base core xml aui xrc html ) # png expat)
endif(UNIX)

if(WIN32)
        add_definitions(/MP) # multiprocess compilation
        set(wxWidgets_REQUIRED_COMPONENTS adv base core xml aui xrc html expat) #  zlib png
endif(WIN32)

#########################################################
#            Compilation OPTION                       ###
#########################################################

option(BUILD_SHARED_LIBS "Dynamic libraries" ON)
option(BUILD_SHARED_EXE  "Dynamic executables" ON)

#########################################################
#            C++0x Variadic template OPTION           ###
#########################################################
option(VARIADIC_TEMPLATES "Enable variadic template implementation" OFF)
if(VARIADIC_TEMPLATES)
  add_definitions("-DUSE_VARIADIC_TEMPLATES=1")
  add_definitions("-std=c++0x")
endif(VARIADIC_TEMPLATES)

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
	add_definitions( ${Boost_LIB_DIAGNOSTIC_DEFINITIONS} )
    if(NOT Boost_USE_STATIC_LIBS)
		add_definitions("/DBOOST_ALL_DYN_LINK" )
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
  add_definitions("-DUSE_CGAL=1")
  set(GEOMETRY_LIBRARIES ${CGAL_LIBRARIES})
else(USE_CGAL)
  set(GEOMETRY_LIBRARIES )
endif(USE_CGAL)


#########################################################
#            Finding OPTIONAL package shapelib        ###
#########################################################
option(USE_SHP "Use SHP library" ON)

if(USE_SHP)
    find_package(shapelib QUIET)
    if(NOT shapelib_FOUND)
		set(shapelib_LIBRARIES "shp" CACHE PATH "SHP library" FORCE  )
		set(shapelib_INCLUDE_DIRS "" CACHE PATH "SHP include directories" FORCE)
	endif(NOT shapelib_FOUND)
    add_definitions("-DUSE_SHP=1")
    add_definitions(${shapelib_DEFINITIONS})
    include_directories(${shapelib_INCLUDE_DIRS})
endif(USE_SHP)

#########################################################
#            Recursion to subdirs                     ###
#########################################################
add_subdirectory(lib)
add_subdirectory(samples)
add_subdirectory(doc)
