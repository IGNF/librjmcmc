CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

#Nom du projet
PROJECT( RJMCMC_Rectangles )

IF(UNIX)
############## Spécial ECLIPSE pour parse correct des headers
INCLUDE_DIRECTORIES(/usr/local/include)
#SET(gcc_version 4.2.4)
#INCLUDE_DIRECTORIES(/usr/lib/gcc/i486-linux-gnu/${gcc_version}/include 
#                   /usr/include/c++ /usr/include/c++/${gcc_version} 
#                    /usr/include/c++/${gcc_version}/backward  
#                    /usr/include/c++/${gcc_version}/i486-linux-gnu
#                    /usr/include/boost
#                    )
############## FIN Spécial ECLIPSE
ENDIF(UNIX)

SET( SRC_DIR ./ )
AUX_SOURCE_DIRECTORY(${SRC_DIR} ALL_SRC )
#SET( XML_SRC_DIR ./xml )
#AUX_SOURCE_DIRECTORY(${XML_SRC_DIR} SRC_XML)

# Find ITK
FIND_PACKAGE(ITK REQUIRED)
IF(ITK_FOUND)
    INCLUDE(${ITK_USE_FILE})
ELSE(ITK_FOUND)
   # For convenience. When we cannot continue, inform the user
   MESSAGE(FATAL_ERROR "ITK not found ! Please set ITK path ...")
ENDIF(ITK_FOUND)

#Find wxWidgets
FIND_PACKAGE(wxWidgets REQUIRED adv base core)
IF(wxWidgets_FOUND)  
	INCLUDE(${wxWidgets_USE_FILE})
ELSE(wxWidgets_FOUND)
	# For convenience. When we cannot continue, inform the user
	MESSAGE(FATAL_ERROR "wxWidgets not found ! Please set wxWidgets path ...")
ENDIF(wxWidgets_FOUND)
		
# Find BOOST
FIND_PACKAGE( Boost 1.34.1 COMPONENTS thread filesystem system )

IF(UNIX)
INCLUDE( CGAL.cmake )
INCLUDE_DIRECTORIES( ${INCLUDE_DIRECTORIES} ${CGAL_INCLUDE_DIRECTORIES} )
# LINK_DIRECTORIES( ${LINK_DIRECTORIES} /usr/local/CGAL/lib )
ENDIF(UNIX)

IF(WIN32)
	SET( CGAL_ROOT_DIR "C:/Program Files/CGAL-3.3.1" )
	INCLUDE_DIRECTORIES( ${CGAL_ROOT_DIR}/include/CGAL/config/msvc )
	INCLUDE_DIRECTORIES( ${CGAL_ROOT_DIR}/include )
	LINK_DIRECTORIES( ${CGAL_ROOT_DIR}/lib )
	#SET( CGAL_LIBRARIES CGALcore++ CGALimageIO CGALPDB )
ENDIF(WIN32)

#SET( ITKVIEWERPATH  ../../ITKViewer/ )
#SET( ITKVIEWERPATH  ../../trunk/ )
SET( ITKVIEWERPATH "" CACHE PATH "Path to ITKViewer" )

LINK_DIRECTORIES( ${LINK_DIRECTORIES} ${ITKVIEWERPATH}/extern/lib )
INCLUDE_DIRECTORIES( ${INCLUDE_DIRECTORIES} ${ITKVIEWERPATH}/src )
INCLUDE_DIRECTORIES( ${INCLUDE_DIRECTORIES} ${ITKVIEWERPATH}/extern )
#INCLUDE_DIRECTORIES( ${INCLUDE_DIRECTORIES} ${ITKVIEWERPATH}/extern/tinyxml_2.5.3 )

#####################################
###                     Fin recherche des packages                         ###
#####################################
IF(UNIX)
#	SET(CMAKE_CXX_FLAGS "-O2 -g -fpermissive")
#	SET(CMAKE_EXE_LINKER_FLAGS "-g")
#	ADD_DEFINITIONS("-pg")
	ADD_DEFINITIONS("-Wall")
	ADD_EXECUTABLE( RJMCMC_Rectangles ${ALL_SRC} ${SRC_XML} )
	TARGET_LINK_LIBRARIES( RJMCMC_Rectangles ${ITK_LIBRARIES}
											 ${wxWidgets_LIBRARIES}
											 ${Boost_LIBRARIES}
											 ${CGAL_LIBRARIES}
											 ITKViewer
											 shapelib
											 tinyxml 
											 )
ENDIF(UNIX)
IF(WIN32)
	ADD_EXECUTABLE( RJMCMC_Rectangles WIN32 ${ALL_SRC} ${SRC_XML} )
ENDIF(WIN32)
	TARGET_LINK_LIBRARIES( RJMCMC_Rectangles ${ITK_LIBRARIES}
											 ${wxWidgets_LIBRARIES}
											 ${Boost_LIBRARIES}
											 ${CGAL_LIBRARIES}
#											 ITKViewer
#											 shapelib
#											 tinyxml 
											)
