find_package( TIFF REQUIRED )
if(TIFF_FOUND)
	include_directories(${TIFF_INCLUDE_DIR})
else()
	message(FATAL_ERROR "TIFF not found ! Please set TIFF path ...")
endif()

file( GLOB ALL_CLI_HEADER_FILES *.h *.hpp )
aux_source_directory( ${CMAKE_CURRENT_SOURCE_DIR} ALL_CLI_SRC )
set( CLI_SRC ${ALL_LIBRJMCMC_HEADERS_FILES} ${ALL_CLI_HEADER_FILES} ${ALL_CLI_SRC} )

add_executable( building_footprint_rectangle_cli ${CLI_SRC} )
target_link_libraries( building_footprint_rectangle_cli ${BUILDING_FOOTPRINT_RECTANGLE_LIBRARIES} ${TIFF_LIBRARIES} ${shapelib_LIBRARIES})
