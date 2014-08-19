file( GLOB_RECURSE rjmcmc_GLOB    ../../include/rjmcmc/*.h    ../../include/rjmcmc/*.hpp    )
add_custom_target(rjmcmc ALL SOURCES ${rjmcmc_GLOB})
set_target_properties(rjmcmc PROPERTIES LINKER_LANGUAGE CXX)
install(DIRECTORY ../../include/rjmcmc DESTINATION include)

# workaround to show rjmcmc header files in qtcreator
add_library(rjmcmc_headers_dummy ${rjmcmc_GLOB})
set_target_properties(rjmcmc_headers_dummy PROPERTIES LINKER_LANGUAGE CXX)
