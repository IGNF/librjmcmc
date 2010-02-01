cmake_minimum_required(VERSION 2.6)

option(BUILD_WXFREECHARTDEMO "Build wxfreechartdemo" OFF)

IF(BUILD_WXFREECHARTDEMO)
	aux_source_directory( . DEMO_SRC )
	add_executable( wxfreechartdemo ${DEMO_SRC} demo.h democollection.h )
	target_link_libraries(wxfreechartdemo ${wxWidgets_LIBRARIES} wxfreechart)
ENDIF()
