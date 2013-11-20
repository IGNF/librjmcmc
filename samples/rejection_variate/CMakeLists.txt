file( GLOB ALL_HEADER_FILES *.h *.hpp )
aux_source_directory( ${CMAKE_CURRENT_SOURCE_DIR} ALL_SOURCE_FILES )
set( SRC ${ALL_LIBRJMCMC_HEADERS_FILES} ${ALL_HEADER_FILES} ${ALL_SOURCE_FILES} )

add_executable( rejection_variate ${SRC} )
