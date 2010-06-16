# Find wxWidgets
find_package( wxWidgets COMPONENTS adv base core xrc aui )
find_package( GilViewer )
if(wxWidgets_FOUND AND GilViewer_FOUND )
    file( GLOB ALL_GUI_HEADER_FILES *.h *.hpp )
    file( GLOB ALL_CORE_HEADER_FILES ../core/*.h ../core/*.hpp )
    aux_source_directory( ${CMAKE_CURRENT_SOURCE_DIR} ALL_GUI_SRC )
    set( GUI_SRC ${ALL_LIBRJMCMC_HEADERS_FILES} ${ALL_GUI_HEADER_FILES} ${ALL_CORE_HEADER_FILES} ${ALL_GUI_SRC} )

    include_directories(${CMAKE_SOURCE_DIR}/lib/wxfreechart/include)
    include(${wxWidgets_USE_FILE})
    add_executable( building_footprint_rectangle_gui ${GUI_SRC} )
    target_link_libraries( building_footprint_rectangle_gui ${BUILDING_FOOTPRINT_RECTANGLE_LIBRARIES} ${wxWidgets_LIBRARIES} GilViewer wxfreechart wx_visitors)
endif()
