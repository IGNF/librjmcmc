cmake_minimum_required(VERSION 2.6)



# Building of Documentation is tested on Linux only
if(CMAKE_SYSTEM_NAME STREQUAL Linux)

# adapted from http://github.com/purpleKarrot/CToolBox.git
  set(THIS_PROJECT_NAME librjmcmc)
  set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake ${CMAKE_MODULE_PATH})
  find_package(Reference)
  find_package(Documentation)

  file( GLOB_RECURSE MPP_HEADER_FILES    ../lib/mpp/*.hpp)
  file( GLOB_RECURSE RJMCMC_HEADER_FILES ../lib/rjmcmc/*.hpp)
  file( GLOB_RECURSE SA_HEADER_FILES     ../lib/simulated_annealing/*.hpp)
  file( GLOB_RECURSE GEOM_HEADER_FILES   ../lib/geometry/*.hpp ../lib/geometry/*.h )
  file( GLOB_RECURSE IMAGE_HEADER_FILES  ../lib/image/*.hpp)
  file( GLOB_RECURSE PARAM_HEADER_FILES  ../lib/param/*.hpp)

  add_reference(librjmcmc-mpp-ref.xml ${MPP_HEADER_FILES}
    BOOSTBOOK boost.doxygen.reftitle="Marked Point Process")
  add_reference(librjmcmc-rjmcmc-ref.xml ${RJMCMC_HEADER_FILES}
    BOOSTBOOK boost.doxygen.reftitle="Reversible-Jump Markov Chain Monte Carlo")
  add_reference(librjmcmc-sa-ref.xml ${SA_HEADER_FILES}
    BOOSTBOOK boost.doxygen.reftitle="Simulated Annealing")
  add_reference(librjmcmc-geom-ref.xml ${GEOM_HEADER_FILES}
    BOOSTBOOK boost.doxygen.reftitle="Geometry Support")
  add_reference(librjmcmc-image-ref.xml ${IMAGE_HEADER_FILES}
    BOOSTBOOK boost.doxygen.reftitle="Image Support")
  add_reference(librjmcmc-param-ref.xml ${PARAM_HEADER_FILES}
    BOOSTBOOK boost.doxygen.reftitle="Parameter Support")

set(QBK_SOURCES
  librjmcmc.qbk
  HTML4_symbols.qbk
  librjmcmc_introduction.qbk
  librjmcmc_compilation.qbk
  librjmcmc_quickstart.qbk
  quickstart.cpp
  librjmcmc_mpp.qbk
  librjmcmc_rjmcmc.qbk
  librjmcmc_simulated_annealing.qbk
  librjmcmc_tutorial.qbk
  librjmcmc_samples.qbk
  samples/sample_multiple_geometries_extraction.qbk
  samples/sample_rectangular_building_footprint_extraction.qbk
  librjmcmc_reference.qbk
  librjmcmc_bibliography.qbk
)

# PARAMETERS : http://docbook.sourceforge.net/release/xsl/current/doc/
  add_documentation(
    ${QBK_SOURCES}
    librjmcmc-mpp-ref.xml
    librjmcmc-rjmcmc-ref.xml
    librjmcmc-sa-ref.xml
    librjmcmc-geom-ref.xml
    librjmcmc-image-ref.xml
    librjmcmc-param-ref.xml
  PARAMETERS
    #boost.root=./
    html.stylesheet=css/boostbook.css
    boost.image.src=images/librjmcmc.png
    boost.image.alt="IGN/MATIS librjmcmc"
    boost.image.w=366
    boost.image.h=200
    chunk.section.depth=4
    #<xsl:param>chunk.first.sections=1
    toc.section.depth=3
    toc.max.depth=2
    generate.section.toc.level=4
    nav.layout=none
    admon.graphics.path=images/admonition/
    navig.graphics.path=images/navigation/
    callout.graphics.path=images/callout/
    #boost.doxygen.strip_from_path=.
    #img.src.path=../images/useless_directory/
  IMAGES
    images/librjmcmc.png
    css/boostbook.css
  )

endif(CMAKE_SYSTEM_NAME STREQUAL Linux)

file( GLOB_RECURSE LIB_HEADER_FILES    ../lib/*.hpp ../lib/*.h)
option(BUILD_QUICKSTART "build quickstart" ON)
if(BUILD_QUICKSTART)
    add_executable( quickstart quickstart.cpp ${LIB_HEADER_FILES}
    ${QBK_SOURCES}
)
    target_link_libraries( quickstart ${Boost_LIBRARIES} ${GEOMETRY_LIBRARIES} ${shapelib_LIBRARIES})
endif(BUILD_QUICKSTART)
