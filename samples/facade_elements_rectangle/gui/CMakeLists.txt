# Find wxWidgets
find_package( wxWidgets COMPONENTS adv base core xrc aui )
find_package( GilViewer )
if(wxWidgets_FOUND AND GilViewer_FOUND )
    file( GLOB ALL_GUI_HEADER_FILES *.h *.hpp )
    aux_source_directory( ${CMAKE_CURRENT_SOURCE_DIR} ALL_GUI_SRC )
    set( GUI_SRC ${ALL_GUI_HEADER_FILES} ${ALL_GUI_SRC} ) 

    include_directories(${CMAKE_SOURCE_DIR}/lib/wxfreechart/include)
    include(${wxWidgets_USE_FILE})
    add_executable( facade_elements_rectangle_gui ${GUI_SRC} )
    target_link_libraries( facade_elements_rectangle_gui ${FACADE_ELEMENTS_RECTANGLE_LIBRARIES} ${wxWidgets_LIBRARIES} GilViewer wxfreechart wx_visitors)
endif()
