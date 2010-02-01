cmake_minimum_required(VERSION 2.6)

aux_source_directory( . MAIN_SRC )
aux_source_directory( axis AXIS_SRC )
aux_source_directory( bars BARS_SRC )
aux_source_directory( category CATEGORY_SRC )
aux_source_directory( gantt GANTT_SRC )
aux_source_directory( ohlc OHLC_SRC )
aux_source_directory( pie PIE_SRC )
aux_source_directory( xy XY_SRC )
aux_source_directory( xy/functions XY_FUNCTIONS_SRC )
aux_source_directory( xyz XYZ_SRC )

add_library(wxfreechart ${wxfreechart_HEADER_FILES} ${MAIN_SRC} ${AXIS_SRC} ${BARS_SRC} ${CATEGORY_SRC} ${GANTT_SRC} ${OHLC_SRC} ${PIE_SRC} ${XY_SRC} ${XY_FUNCTIONS_SRC} ${XYZ_SRC})
