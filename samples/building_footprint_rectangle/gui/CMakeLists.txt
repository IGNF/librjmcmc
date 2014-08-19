include_directories(${rjmcmc_INCLUDE_DIRS})
add_definitions(${rjmcmc_DEFINITIONS})
include_directories(../core)

if(WIN32)
  set(exe_FLAG "WIN32")
endif()

file( GLOB SRCS *.c *.cpp *.cxx *.h *.hpp *.hxx)
add_executable( building_footprint_rectangle_gui ${exe_FLAG} ${SRCS} )
target_link_libraries( building_footprint_rectangle_gui ${rjmcmc_LIBRARIES})

