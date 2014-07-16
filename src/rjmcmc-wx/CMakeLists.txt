if(UNIX)
        set(wxWidgets_REQUIRED_COMPONENTS adv base core xml aui xrc html )
endif(UNIX)
if(WIN32)
        set(wxWidgets_REQUIRED_COMPONENTS adv base core xml aui xrc html expat)
endif(WIN32)

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
    

    include_directories(${CMAKE_SOURCE_DIR}/extern/wxfreechart/include)
    include_directories(${CMAKE_SOURCE_DIR}/include)
    aux_source_directory( . rjmcmc-wx_CPP )
    add_library(rjmcmc-wx STATIC ${rjmcmc-wx_CPP} ${rjmcmc-wx_HPP})
    target_link_libraries(rjmcmc-wx wxfreechart)

    install(TARGETS rjmcmc-wx
        EXPORT rjmcmc-wx-targets
        DESTINATION ${CMAKE_INSTALL_PREFIX}/lib
    )

endif()
