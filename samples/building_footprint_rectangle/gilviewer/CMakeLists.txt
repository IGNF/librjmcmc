# Find GilViewer
find_package( GilViewer REQUIRED)
include_directories(${GilViewer_INCLUDE_DIRS})
link_directories(${GilViewer_LIBRARY_DIRS})
link_directories(${GilViewer_LIBRARY_DIR})

# Find wxWidgets
set(wxWidgets_EXCLUDE_COMMON_LIBRARIES TRUE)
if(UNIX)
        find_package(wxWidgets REQUIRED adv base core xml aui xrc html REQUIRED)
else()
        find_package(wxWidgets REQUIRED adv base core xml aui xrc html expat REQUIRED)
        add_definitions(${wxWidgets_DEFINITIONS})
endif()
include(${wxWidgets_USE_FILE})


# use wxfreechart
include_directories(${CMAKE_SOURCE_DIR}/lib/wxfreechart/include)

file( GLOB SRCS *.c *.cpp *.cxx *.h *.hpp *.hxx)
add_library(GilViewer-building_footprint_rectangle SHARED ${SRCS})

target_link_libraries(GilViewer-building_footprint_rectangle ${BUILDING_FOOTPRINT_RECTANGLE_LIBRARIES} ${Boost_LIBRARIES} ${wxWidgets_LIBRARIES} GilViewer tinyxml wxfreechart wx_visitors boost_program_options)

install(TARGETS GilViewer-building_footprint_rectangle
    EXPORT GilViewer-targets
    DESTINATION ${GilViewer_LIBRARY_DIR}
)
