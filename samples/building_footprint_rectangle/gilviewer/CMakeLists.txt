include_directories(${rjmcmc_INCLUDE_DIRS})
add_definitions(${rjmcmc_DEFINITIONS})
include_directories(../core)

file( GLOB SRCS *.c *.cpp *.cxx *.h *.hpp *.hxx)
add_library(GilViewer-building_footprint_rectangle SHARED ${SRCS})
target_link_libraries(GilViewer-building_footprint_rectangle ${rjmcmc_LIBRARIES})

install(TARGETS GilViewer-building_footprint_rectangle
  EXPORT GilViewer-targets
  DESTINATION ${GilViewer_LIBRARY_DIR}
)
