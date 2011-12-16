find_package(wxWidgets REQUIRED ${wxWidgets_REQUIRED_COMPONENTS})
find_package( GilViewer )

if(GilViewer_FOUND)
	include_directories(${GilViewer_INCLUDE_DIRS})
else()
	message(FATAL_ERROR "GilViewer NOT found!")
endif()

if(wxWidgets_FOUND)
    include(${wxWidgets_USE_FILE})
	
    add_definitions("-D_LOCAL_")
    
    file( GLOB WX_VISITORS_HEADER_FILES *.h *.hpp )
    file( GLOB VISITORS_HEADER_FILES ../*.h ../*.hpp )
    include_directories(${CMAKE_SOURCE_DIR}/lib/wxfreechart/include)
    aux_source_directory( . WX_VISITORS_SRC )
    add_library(wx_visitors STATIC ${WX_VISITORS_HEADER_FILES} ${VISITORS_HEADER_FILES} ${WX_VISITORS_SRC})
    target_link_libraries(wx_visitors wxfreechart)
endif()
