cmake_minimum_required(VERSION 2.6)

# cmake version 2.8.0
#  CMP0014
#       Input directories must have CMakeLists.txt.
#       CMake versions before 2.8 silently ignored missing CMakeLists.txt
#       files in directories referenced by add_subdirectory() or subdirs(),
#       treating them as if present but empty.  In CMake 2.8.0 and above this
#       policy determines whether or not the case is an error.  The OLD
#       behavior for this policy is to silently ignore the problem.  The NEW
#       behavior for this policy is to report an error.
#       This policy was introduced in CMake version 2.8.0.  CMake version
#       2.8.0 warns when the policy is not set and uses OLD behavior.  Use the
#       cmake_policy command to set it to OLD or NEW explicitly.
#cmake_policy(SET CMP0014 OLD)

# Project name
project( librjmcmc CXX)

set( ROOT_DIR ${PROJECT_SOURCE_DIR})
# SET classical path for the MATIS library

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

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin)
set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR}/lib)

option(BUILD_SHARED_LIBS "Dynamic libraries" ON)
option(BUILD_SHARED_EXE  "Dynamic executables" ON)

# Find BOOST
set( Boost_ADDITIONAL_VERSIONS "1.39.0" "1.39" "1.40.0" "1.40" "1.41.0" "1.41" "1.42.0" "1.42" )
find_package( Boost 1.37 COMPONENTS thread program_options system filesystem )
if( NOT Boost_FOUND )
	message(FATAL_ERROR "Boost not found ! Please set Boost path ...")
else()
	include_directories( ${Boost_INCLUDE_DIRS} )
	link_directories( ${Boost_LIBRARY_DIRS} )
endif()


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
  set(GEOMETRY_LIBRARIES geometry)
endif(USE_CGAL)

include_directories( lib )

add_subdirectory(lib)
add_subdirectory(samples)
