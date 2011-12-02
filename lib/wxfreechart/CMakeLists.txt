cmake_minimum_required(VERSION 2.6)

find_package(wxWidgets REQUIRED adv base core xml aui xrc html zlib png expat)
INCLUDE(${wxWidgets_USE_FILE})

include_directories( include )
file( GLOB WXFREECHART_HEADERS include/wx/*.h include/wx/*/*.h )
add_subdirectory(sample)
add_subdirectory(src)
