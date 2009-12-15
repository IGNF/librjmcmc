# Find wxWidgets
find_package( wxWidgets COMPONENTS adv base core xrc aui )
find_package( GilViewer )
if(wxWidgets_FOUND AND GilViewer_FOUND )
    add_definitions( ${wxWidgets_DEFINITIONS}  -DUSE_GUI=1 )
    include(${wxWidgets_USE_FILE})
    add_executable( building_footprint_extraction_gui ${SRC} ${GUI_SRC} )
    target_link_libraries( building_footprint_extraction_gui ${BUILDING_FOOTPRINT_EXTRACTION_LIBRARIES} ${wxWidgets_LIBRARIES} GilViewer )
endif()
