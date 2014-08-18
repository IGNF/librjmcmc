file( GLOB_RECURSE rjmcmc_GLOB    rjmcmc/*.h    rjmcmc/*.hpp    )
file( GLOB_RECURSE rjmcmc-wx_GLOB rjmcmc-wx/*.h rjmcmc-wx/*.hpp )
set(rjmcmc_HPP    ${rjmcmc_GLOB}    PARENT_SCOPE)
set(rjmcmc-wx_HPP ${rjmcmc-wx_GLOB} PARENT_SCOPE)

install(DIRECTORY ./ DESTINATION include)
configure_file(rjmcmcConfig.cmake.in ${CMAKE_BINARY_DIR}/rjmcmcConfig.cmake.in2 @ONLY)
configure_file(${CMAKE_BINARY_DIR}/rjmcmcConfig.cmake.in2 ${CMAKE_BINARY_DIR}/rjmcmcConfig.cmake @ONLY)
install (FILES ${CMAKE_BINARY_DIR}/rjmcmcConfig.cmake DESTINATION ${CMAKE_INSTALL_PREFIX}/cmake)
