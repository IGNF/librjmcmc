cmake_minimum_required(VERSION 2.8)
project(wxfreechart CXX)

set(wxfreechart_MAJOR_VERSION 1)
set(wxfreechart_MINOR_VERSION 4)
set(wxfreechart_PATCH_VERSION 0)
set(wxfreechart_VERSION
  ${wxfreechart_MAJOR_VERSION}.${wxfreechart_MINOR_VERSION}.${wxfreechart_PATCH_VERSION})

if(WIN32 AND NOT CYGWIN)
  set(wxfreechart_DEF_INSTALL_CMAKE_DIR cmake)
else()
  set(wxfreechart_DEF_INSTALL_CMAKE_DIR lib/cmake/wxfreechart)
endif()

# Offer the user the choice of overriding the installation directories
set(wxfreechart_INSTALL_LIB_DIR     lib     CACHE PATH "Installation directory for libraries"   )
set(wxfreechart_INSTALL_BIN_DIR     bin     CACHE PATH "Installation directory for executables" )
set(wxfreechart_INSTALL_INCLUDE_DIR include CACHE PATH "Installation directory for header files")
set(wxfreechart_INSTALL_CMAKE_DIR ${DEF_INSTALL_CMAKE_DIR} CACHE PATH
  "Installation directory for CMake files")

# Make relative paths absolute (needed later on)
foreach(p LIB BIN INCLUDE CMAKE)
  set(var wxfreechart_INSTALL_${p}_DIR)
  if(NOT IS_ABSOLUTE "${${var}}")
    set(${var} "${CMAKE_INSTALL_PREFIX}/${${var}}")
  endif()
endforeach()
 
# set up include-directories
include_directories(include) 
 
add_subdirectory(sample)
add_subdirectory(src)
 
# Add all targets to the build-tree export set
export(TARGETS wxfreechart FILE "${PROJECT_BINARY_DIR}/wxfreechart-targets.cmake")
 
# Export the package for use from the build-tree
# (this registers the build-tree with a global CMake-registry)
export(PACKAGE wxfreechart)
 
# Create the wxfreechart-config.cmake and wxfreechart-config-version files
file(RELATIVE_PATH REL_INCLUDE_DIR "${wxfreechart_INSTALL_CMAKE_DIR}" "${wxfreechart_INSTALL_INCLUDE_DIR}")

# ... for the build tree
set(CONF_INCLUDE_DIRS "${PROJECT_SOURCE_DIR}/include")
configure_file(wxfreechart-config.cmake.in "${PROJECT_BINARY_DIR}/wxfreechart-config.cmake" @ONLY)

# ... for the install tree
set(CONF_INCLUDE_DIRS "\${wxfreechart_INSTALL_CMAKE_DIR}/${REL_INCLUDE_DIR}")
configure_file(wxfreechart-config.cmake.in "${PROJECT_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/wxfreechart-config.cmake" @ONLY)

# ... for both
configure_file(wxfreechart-config-version.cmake.in "${PROJECT_BINARY_DIR}/wxfreechart-config-version.cmake" @ONLY)
 
# Install the wxfreechart-config.cmake and wxfreechart-config-version.cmake
install(FILES
  "${PROJECT_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/wxfreechart-config.cmake"
  "${PROJECT_BINARY_DIR}/wxfreechart-config-version.cmake"
  DESTINATION "${wxfreechart_INSTALL_CMAKE_DIR}" COMPONENT dev)
 
# Install the export set for use with the install-tree
install(EXPORT wxfreechart-targets DESTINATION
  "${wxfreechart_INSTALL_CMAKE_DIR}" COMPONENT dev)
