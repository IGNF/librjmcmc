file( GLOB_RECURSE rjmcmc_GLOB    ../../include/rjmcmc/*.h    ../../include/rjmcmc/*.hpp    )
add_custom_target(rjmcmc ALL SOURCES ${rjmcmc_GLOB})
set_target_properties(rjmcmc PROPERTIES LINKER_LANGUAGE CXX)
install(DIRECTORY ../../include/rjmcmc DESTINATION include)
