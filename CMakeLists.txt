CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

#Nom du projet
PROJECT( RJMCMC_Rectangles )

IF(UNIX)
############## Spécial ECLIPSE pour parse correct des headers
INCLUDE_DIRECTORIES(/usr/local/include)
#SET(gcc_version 4.2.4)
#INCLUDE_DIRECTORIES(	/usr/lib/gcc/i486-linux-gnu/${gcc_version}/include
#						/usr/include/c++ /usr/include/c++/${gcc_version}
#						/usr/include/c++/${gcc_version}/backward
#						/usr/include/c++/${gcc_version}/i486-linux-gnu
#						/usr/include/boost
#					)
############## FIN Spécial ECLIPSE
ENDIF(UNIX)

SET( SRC_DIR ./ )
AUX_SOURCE_DIRECTORY(${SRC_DIR} ALL_SRC )

SET( SRC_NO_IHM ${SRC_DIR}BuildingsDetectorParameters.cpp
				${SRC_DIR}RectangleNode.cpp
				${SRC_DIR}Main_RJMCMC_BuildingsDetection_noihm.cpp
	)
	
SET( SRC_IHM	${SRC_DIR}BuildingsDetectorParameters.cpp
				${SRC_DIR}RectangleNode.cpp
				${SRC_DIR}Main_RJMCMC_BuildingsDetection_ihm.cpp
				${SRC_DIR}RJMCMC_BuildingsDetectionFrame.cpp
	)

# Find ITK
FIND_PACKAGE(ITK REQUIRED)
IF(ITK_FOUND)
    INCLUDE(${ITK_USE_FILE})
ELSE(ITK_FOUND)
   # For convenience. When we cannot continue, inform the user
   MESSAGE(FATAL_ERROR "ITK not found ! Please set ITK path ...")
ENDIF(ITK_FOUND)


# Find BOOST
FIND_PACKAGE( Boost 1.37 COMPONENTS program_options thread filesystem system )

IF(UNIX)
	INCLUDE( CGAL.cmake )
	INCLUDE_DIRECTORIES( ${INCLUDE_DIRECTORIES} ${CGAL_INCLUDE_DIRECTORIES} )
#	LINK_DIRECTORIES( ${LINK_DIRECTORIES} /usr/local/CGAL/lib )
ENDIF(UNIX)

IF(WIN32)
	SET( CGAL_ROOT_DIR "C:/Program Files/CGAL-3.3.1" )
	INCLUDE_DIRECTORIES( ${CGAL_ROOT_DIR}/include/CGAL/config/msvc )
	INCLUDE_DIRECTORIES( ${CGAL_ROOT_DIR}/include )
	LINK_DIRECTORIES( ${CGAL_ROOT_DIR}/lib )
ENDIF(WIN32)

#####################################
###                     Fin recherche des packages                         ###
#####################################

#####################################
###                     Executable sans IHM                        ###
#####################################
ADD_EXECUTABLE( RJMCMC_Rectangles_noihm ${SRC_NO_IHM} )
TARGET_LINK_LIBRARIES( RJMCMC_Rectangles_noihm	${ITK_LIBRARIES}
										 		${Boost_LIBRARIES}
										 		${CGAL_LIBRARIES}
										)

#####################################
###                     Executable avec IHM                        ###
#####################################

#Find wxWidgets
FIND_PACKAGE(wxWidgets REQUIRED adv base core)
IF(wxWidgets_FOUND)  
	INCLUDE(${wxWidgets_USE_FILE})
ELSE(wxWidgets_FOUND)
	# For convenience. When we cannot continue, inform the user
	MESSAGE(FATAL_ERROR "wxWidgets not found ! Please set wxWidgets path ...")
ENDIF(wxWidgets_FOUND)

SET( ITKVIEWERPATH "" CACHE PATH "Path to ITKViewer" )

INCLUDE_DIRECTORIES( ${ITKVIEWERPATH} )
INCLUDE_DIRECTORIES( ${ITKVIEWERPATH}/../extern )
LINK_DIRECTORIES( ${ITKVIEWERPATH}/../extern/lib )

IF(UNIX)
	ADD_DEFINITIONS("-Wall")
	ADD_EXECUTABLE( RJMCMC_Rectangles_ihm ${SRC_IHM}  )

	TARGET_LINK_LIBRARIES( RJMCMC_Rectangles_ihm	${ITK_LIBRARIES}
													${wxWidgets_LIBRARIES}
													${Boost_LIBRARIES}
													${CGAL_LIBRARIES}
													ITKViewer
													shapelib
													tinyxml 
											 )
ENDIF(UNIX)
IF(WIN32)
	ADD_EXECUTABLE( RJMCMC_Rectangles_ihm WIN32 ${SRC_IHM}  )
ENDIF(WIN32)


TARGET_LINK_LIBRARIES( RJMCMC_Rectangles_ihm ${ITK_LIBRARIES}
										 ${Boost_LIBRARIES}
										 ${CGAL_LIBRARIES}
										 ${wxWidgets_LIBRARIES}
										 ITKViewer
										 shapelib
										 tinyxml 
										)
