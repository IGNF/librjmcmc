
find_package(wxWidgets REQUIRED ${wxWidgets_REQUIRED_COMPONENTS})
INCLUDE(${wxWidgets_USE_FILE})

include_directories( ../include )
file( GLOB wxfreechart_HEADERS ../include/wx/*.h ../include/wx/*/*.h )

aux_source_directory( . MAIN_SRC )
set(wxfreechart_SRC ${MAIN_SRC})

macro(wxfreechart_add_module src  module MODULE default)
  option(wxfreechart_${MODULE} "Building wxfreechart module ${module}" ${default})
  if(wxfreechart_${MODULE})
    aux_source_directory( ${module} ${MODULE}_SRC )
    set(${src} ${${src}} ${${MODULE}_SRC})
  endif()
  mark_as_advanced(wxfreechart_${MODULE})
endmacro(wxfreechart_add_module)

wxfreechart_add_module(wxfreechart_SRC axis         AXIS         1)
wxfreechart_add_module(wxfreechart_SRC category     CATEGORY     1)
wxfreechart_add_module(wxfreechart_SRC bars         BARS         1)
wxfreechart_add_module(wxfreechart_SRC xy           XY           1)
wxfreechart_add_module(wxfreechart_SRC xy/functions XY_FUNCTIONS 0)
wxfreechart_add_module(wxfreechart_SRC xyz          XYZ          0)
wxfreechart_add_module(wxfreechart_SRC gantt        GANTT        0)
wxfreechart_add_module(wxfreechart_SRC ohlc         OHLC         0)
wxfreechart_add_module(wxfreechart_SRC pie          PIE          0)

add_definitions("-D_LOCAL_")


if(UNIX)
  set(wxfreechart_LINK SHARED)
else()
  set(wxfreechart_LINK STATIC)
endif()

add_library(wxfreechart ${wxfreechart_LINK} ${wxfreechart_SRC} ${wxfreechart_HEADERS})

set_target_properties(wxfreechart PROPERTIES PUBLIC_HEADER "${wxfreechart_HEADERS}")

target_link_libraries(wxfreechart ${wxWidgets_LIBRARIES})

install(TARGETS wxfreechart EXPORT wxfreechart-targets
  RUNTIME DESTINATION ${wxfreechart_INSTALL_BIN_DIR} COMPONENT bin
  LIBRARY DESTINATION ${wxfreechart_INSTALL_LIB_DIR} COMPONENT lib
  PUBLIC_HEADER DESTINATION "${wxfreechart_INSTALL_INCLUDE_DIR}/wxfreechart" COMPONENT dev)


