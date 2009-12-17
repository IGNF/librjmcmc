# Find wxWidgets
find_package( wxWidgets COMPONENTS adv base core xrc aui )
find_package( GilViewer )
if(wxWidgets_FOUND AND GilViewer_FOUND )
    file( GLOB ALL_GUI_HEADER_FILES *.h *.hpp )
    aux_source_directory( ${CMAKE_CURRENT_SOURCE_DIR} ALL_GUI_SRC )
    set( GUI_SRC ${ALL_GUI_HEADER_FILES} ${ALL_GUI_SRC} ) 

    add_definitions( ${wxWidgets_DEFINITIONS} )
    include(${wxWidgets_USE_FILE})
    add_executable( building_footprint_extraction_gui ${GUI_SRC} )
    target_link_libraries( building_footprint_extraction_gui ${BUILDING_FOOTPRINT_EXTRACTION_LIBRARIES} ${wxWidgets_LIBRARIES} GilViewer )
endif()
