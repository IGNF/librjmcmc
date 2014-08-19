# set up platform specific variables
if(UNIX)
  set(wxWidgets_REQUIRED_COMPONENTS adv base core xml aui xrc html )
  set(rjmcmc-wx_DEFINITIONS "-fPIC")
endif()

if(WIN32)
  set(wxWidgets_REQUIRED_COMPONENTS adv base core xml aui xrc html expat)
endif()

# find dependent packages
find_package(wxWidgets QUIET COMPONENTS ${wxWidgets_REQUIRED_COMPONENTS})
find_package(wxfreechart QUIET)
find_package(GilViewer QUIET)

if(NOT wxWidgets_FOUND)
  message(WARNING "Could not find wxWidgets, disabling librjmcmc-wx support.")
  set(rjmcmc_BUILD_wx   OFF   CACHE BOOL "build the rjmcmc-wx support library" FORCE)
endif()
if(NOT wxfreechart_FOUND)
  message(WARNING "Could not find wxfreechart, disabling librjmcmc-wx support.")
  set(rjmcmc_BUILD_wx   OFF   CACHE BOOL "build the rjmcmc-wx support library" FORCE)
endif()
if(NOT GilViewer_FOUND)
  message(WARNING "Could not find GilViewer, disabling librjmcmc-wx support.")
  set(rjmcmc_BUILD_wx   OFF   CACHE BOOL "build the rjmcmc-wx support library" FORCE)
endif()

if(rjmcmc_BUILD_wx)

# prepend "-D" to the wxWidgets definitions
set(wxWidgets_DEFINITIONS0 ${wxWidgets_DEFINITIONS})
set(wxWidgets_DEFINITIONS )
foreach(def ${wxWidgets_DEFINITIONS0})
  list(APPEND wxWidgets_DEFINITIONS "-D${def}")
endforeach()

# aggregate INCLUDE_DIRS, LIBRARIES and DEFINITIONS
list(APPEND rjmcmc-wx_INCLUDE_DIRS ${wxWidgets_INCLUDE_DIRS})
list(APPEND rjmcmc-wx_INCLUDE_DIRS ${GilViewer_INCLUDE_DIRS})
list(APPEND rjmcmc-wx_INCLUDE_DIRS ${wxfreechart_INCLUDE_DIRS})
list(APPEND rjmcmc-wx_LIBRARIES    ${wxWidgets_LIBRARIES})
list(APPEND rjmcmc-wx_LIBRARIES    "GilViewer")
list(APPEND rjmcmc-wx_LIBRARIES    ${wxfreechart_LIBRARIES})
list(APPEND rjmcmc-wx_DEFINITIONS  ${wxWidgets_DEFINITIONS})
list(APPEND rjmcmc-wx_DEFINITIONS  ${GilViewer_DEFINITIONS})
list(APPEND rjmcmc-wx_DEFINITIONS  ${wxfreechart_DEFINITIONS})

# make it available to parent scope for config.cmake generation
set(rjmcmc-wx_INCLUDE_DIRS ${rjmcmc-wx_INCLUDE_DIRS} PARENT_SCOPE)
set(rjmcmc-wx_LIBRARIES    ${rjmcmc-wx_LIBRARIES}    PARENT_SCOPE)
set(rjmcmc-wx_DEFINITIONS  ${rjmcmc-wx_DEFINITIONS}  PARENT_SCOPE)

# build the rjmcmc-wx library
add_definitions(${rjmcmc_DEFINITIONS})
add_definitions(${rjmcmc-wx_DEFINITIONS})
include_directories(${rjmcmc_INCLUDE_DIRS})
include_directories(${rjmcmc-wx_INCLUDE_DIRS})
include_directories(../../include)

file( GLOB_RECURSE rjmcmc-wx_HPP    ../../include/rjmcmc-wx/*.h  ../../include/rjmcmc-wx/*.hpp    )
aux_source_directory( . rjmcmc-wx_CPP )
add_library(rjmcmc-wx STATIC ${rjmcmc-wx_CPP} ${rjmcmc-wx_HPP})
target_link_libraries(rjmcmc-wx ${rjmcmc_LIBRARIES} ${rjmcmc-wx_LIBRARIES})

# install the rjmcmc-wx header files
install(DIRECTORY ../../include/rjmcmc-wx
  DESTINATION include
  COMPONENT wx
)

# export the rjmcmc-wx target
install(TARGETS rjmcmc-wx EXPORT rjmcmc-targets  DESTINATION "${rjmcmc_INSTALL_LIB_DIR}" COMPONENT wx)

endif()
