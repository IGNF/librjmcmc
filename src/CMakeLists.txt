cmake_minimum_required(VERSION 2.8)

set(rjmcmc_MAJOR_VERSION 0)
set(rjmcmc_MINOR_VERSION 2)
set(rjmcmc_PATCH_VERSION 0)
set(rjmcmc_VERSION
  ${rjmcmc_MAJOR_VERSION}.${rjmcmc_MINOR_VERSION}.${rjmcmc_PATCH_VERSION})

if(WIN32 AND NOT CYGWIN)
  set(rjmcmc_DEF_INSTALL_CMAKE_DIR cmake)
else()
  set(rjmcmc_DEF_INSTALL_CMAKE_DIR lib/cmake/rjmcmc)
endif()

# Offer the user the choice of overriding the installation directories
set(rjmcmc_DIR                 "${PROJECT_BINARY_DIR}" CACHE PATH "rjmcmc build directory")
set(rjmcmc_INSTALL_LIB_DIR     lib     CACHE PATH "Installation directory for libraries"   )
set(rjmcmc_INSTALL_BIN_DIR     bin     CACHE PATH "Installation directory for executables" )
set(rjmcmc_INSTALL_INCLUDE_DIR include CACHE PATH "Installation directory for header files")
set(rjmcmc_INSTALL_CMAKE_DIR ${rjmcmc_DEF_INSTALL_CMAKE_DIR} CACHE PATH
  "Installation directory for CMake files")

# Make relative paths absolute (needed later on)
foreach(p LIB BIN INCLUDE CMAKE)
  set(var rjmcmc_INSTALL_${p}_DIR)
  if(NOT IS_ABSOLUTE "${${var}}")
    set(${var} "${CMAKE_INSTALL_PREFIX}/${${var}}")
  endif()
endforeach()

# set up include-directories
include_directories(../include)

add_subdirectory(rjmcmc)
add_subdirectory(rjmcmc-wx)

# Add all targets to the build-tree export set
#export(TARGETS rjmcmc-wx FILE "${PROJECT_BINARY_DIR}/rjmcmc-targets.cmake")

# Export the package for use from the build-tree
# (this registers the build-tree with a global CMake-registry)
export(PACKAGE rjmcmc)
 
# Create the rjmcmc-config.cmake and rjmcmc-config-version files
file(RELATIVE_PATH REL_INCLUDE_DIR "${rjmcmc_INSTALL_CMAKE_DIR}" "${rjmcmc_INSTALL_INCLUDE_DIR}")

# ... for the build tree
set(CONF_INCLUDE_DIRS "${PROJECT_SOURCE_DIR}/include")
configure_file(rjmcmc-config.cmake.in "${PROJECT_BINARY_DIR}/rjmcmc-config.cmake" @ONLY)

# ... for the install tree
set(CONF_INCLUDE_DIRS "\${rjmcmc_INSTALL_CMAKE_DIR}/${REL_INCLUDE_DIR}")
configure_file(rjmcmc-config.cmake.in "${PROJECT_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/rjmcmc-config.cmake" @ONLY)

# ... for both
configure_file(rjmcmc-config-version.cmake.in "${PROJECT_BINARY_DIR}/rjmcmc-config-version.cmake" @ONLY)
 
# Install the rjmcmc-config.cmake and rjmcmc-config-version.cmake
install(FILES
  "${PROJECT_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/rjmcmc-config.cmake"
  "${PROJECT_BINARY_DIR}/rjmcmc-config-version.cmake"
  DESTINATION "${rjmcmc_INSTALL_CMAKE_DIR}" COMPONENT dev)
 
# Install the export set for use with the install-tree
install(EXPORT rjmcmc-targets DESTINATION "${rjmcmc_INSTALL_CMAKE_DIR}" COMPONENT dev)
