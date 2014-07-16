cmake_minimum_required(VERSION 2.6)

aux_source_directory( . MAIN_SRC )
set(WXFREECHART_SRC ${MAIN_SRC})

macro(wxfreechart_add_module src  module MODULE default)
  option(WXFREECHART_${MODULE} "Building wxfreechart module ${module}" ${default})
  if(WXFREECHART_${MODULE})
    aux_source_directory( ${module} ${MODULE}_SRC )
    set(${src} ${${src}} ${${MODULE}_SRC})
  endif()
  mark_as_advanced(WXFREECHART_${MODULE})
endmacro(wxfreechart_add_module)

wxfreechart_add_module(WXFREECHART_SRC axis         AXIS         1)
wxfreechart_add_module(WXFREECHART_SRC category     CATEGORY     1)
wxfreechart_add_module(WXFREECHART_SRC bars         BARS         1)
wxfreechart_add_module(WXFREECHART_SRC xy           XY           1)
wxfreechart_add_module(WXFREECHART_SRC xy/functions XY_FUNCTIONS 0)
wxfreechart_add_module(WXFREECHART_SRC xyz          XYZ          0)
wxfreechart_add_module(WXFREECHART_SRC gantt        GANTT        0)
wxfreechart_add_module(WXFREECHART_SRC ohlc         OHLC         0)
wxfreechart_add_module(WXFREECHART_SRC pie          PIE          0)

add_definitions("-D_LOCAL_")

if(UNIX)
    add_library(wxfreechart ${WXFREECHART_HEADERS} ${WXFREECHART_SRC})
else()
    add_library(wxfreechart STATIC ${WXFREECHART_HEADERS} ${WXFREECHART_SRC})
endif()

message(STATUS ${wxWidgets_LIBRARIES})

target_link_libraries(wxfreechart ${wxWidgets_LIBRARIES})

install(TARGETS wxfreechart
    EXPORT wxfreechart-targets
    DESTINATION ${CMAKE_INSTALL_PREFIX}/lib
)

