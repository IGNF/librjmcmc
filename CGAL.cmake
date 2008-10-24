# This file contains CGAL makefile settings for the following platform:
# OS:				i686_Linux-2.6
# COMPILER:			GNU 4.2.3
# BOOST:	supported (1.34.1)
# BOOST_PROGRAM_OPTIONS:	supported (1.34.1)
# BOOST_BIMAP:	not supported
# X11:	supported (11.0)
# GMP:	supported (4.2.2)
# GMPXX:	supported (4.2.2)
# MPFR:	supported (2.3.1)
# ZLIB:	supported (1.2.3.3)
# BLAS:	supported (unknown version)
# LAPACK:	supported (unknown version)
# ATLAS:	not supported
# LEDA:	not supported
# LEDAWIN:	not supported
# OPENGL:	supported (2.1)
# QT:	not supported
# TAUCS:	not supported

# Fichier cree a partir du makefile genere par l'installeur de CGAL ...

SET( SPACE "" )

#---------------------------------------------------------------------#
#                    include/lib directory settings
#---------------------------------------------------------------------#

SET( CGAL_INCL_DIR /home/olivier/work/dev/lib/CGAL-3.3.1/include )
SET( CGAL_LIB_DIR /home/olivier/work/dev/lib/CGAL-3.3.1/lib )

#---------------------------------------------------------------------#
#                    os/compiler description
#---------------------------------------------------------------------#

SET( CGAL_OS_COMPILER i686_Linux-2.6_g++-4.2.3 )

#---------------------------------------------------------------------#
#                    filename related flags
#---------------------------------------------------------------------#

SET( OBJ_EXT .o )
#EXE_EXT = 
#LIB_SUFFIX = 

#---------------------------------------------------------------------#
#                    some specific compiler flags
#---------------------------------------------------------------------#

#OBJ_OPT = -c
#DEBUG_OPT = -g
#START_LD_OPT = 
#LIBPATH_OPT = -L
#LIB_OPT =  -l
#RANLIB = /bin/cat >/dev/null

#---------------------------------------------------------------------#
#                    suffixes and rules
#---------------------------------------------------------------------#

#.SUFFIXES:   .cpp .clean

#.C.clean:
#	rm -f $* $*.o $*.moc


#.cpp.clean:
#	rm -f $* $*.o $*.moc


#---------------------------------------------------------------------#
#                    compiler
#---------------------------------------------------------------------#

SET( CGAL_CXX /usr/bin/g++ )

#---------------------------------------------------------------------#
#                    compiler flags
#---------------------------------------------------------------------#

# For more information about the compiler flags see the installation guide.

# compiler specific configuration dir
SET( CGAL_INCL_CONF_DIR ${CGAL_INCL_DIR}/CGAL/config/i686_Linux-2.6_g++-4.2.3 )

# *** Fill in any additional compiler flags you need ***
# CUSTOM_CXXFLAGS = 

SET( CGAL_SHARED_LIB_EXT .so )

# *** compiler flags for feature BOOST ***
SET( BOOST_CXXFLAGS "" )

# *** compiler flags for feature BOOST_PROGRAM_OPTIONS ***
SET( BOOST_PROGRAM_OPTIONS_CXXFLAGS "")

# *** compiler flags for feature X11 ***
SET( X11_CXXFLAGS "" )

# *** compiler flags for feature GMP ***
SET( GMP_CXXFLAGS "" )

# *** compiler flags for feature GMPXX ***
SET( GMPXX_CXXFLAGS "" )

# *** compiler flags for feature MPFR ***
SET( MPFR_CXXFLAGS "" )

# *** compiler flags for feature ZLIB ***
SET( ZLIB_CXXFLAGS "" )

# *** compiler flags for feature BLAS ***
SET( BLAS_CXXFLAGS "" )

# *** compiler flags for feature LAPACK ***
SET( LAPACK_CXXFLAGS "" )

# *** compiler flags for feature OPENGL ***
SET( OPENGL_CXXFLAGS "" )

# *** compiler flags for package BOOST ***
SET( BOOST_PKG_CXXFLAGS "" )

# *** compiler flags for package BOOST_PROGRAM_OPTIONS ***
SET( BOOST_PROGRAM_OPTIONS_PKG_CXXFLAGS "" )

# *** compiler flags for package X11 ***
SET( X11_PKG_CXXFLAGS "" )

# *** compiler flags for package GMP ***
SET( GMP_PKG_CXXFLAGS "" )

# *** compiler flags for package GMPXX ***
SET( GMPXX_PKG_CXXFLAGS "" )

# *** compiler flags for package MPFR ***
SET( MPFR_PKG_CXXFLAGS "" )

# *** compiler flags for package ZLIB ***
SET( ZLIB_PKG_CXXFLAGS "" )

# *** compiler flags for package REFBLASGCC ***
SET( REFBLASGCC_PKG_CXXFLAGS -DCGAL_USE_F2C )
ADD_DEFINITIONS( ${REFBLASGCC_PKG_CXXFLAGS} )

# *** compiler flags for package OPENGL ***
SET( OPENGL_PKG_CXXFLAGS "" )

# *** compiler flags for package DEFAULTLAPACK ***
SET( DEFAULTLAPACK_PKG_CXXFLAGS -DCGAL_USE_F2C )
ADD_DEFINITIONS( ${DEFAULTLAPACK_PKG_CXXFLAGS} )

# *** complete compiler flags for feature BOOST ***
SET( ALL_BOOST_CXXFLAGS ${BOOST_CXXFLAGS} ${SPACE} ${BOOST_PKG_CXXFLAGS} )
ADD_DEFINITIONS( ${ALL_BOOST_CXXFLAGS} )

# *** complete compiler flags for feature BOOST_PROGRAM_OPTIONS ***
SET( ALL_BOOST_PROGRAM_OPTIONS_CXXFLAGS ${BOOST_PROGRAM_OPTIONS_CXXFLAGS} ${SPACE} ${BOOST_PROGRAM_OPTIONS_PKG_CXXFLAGS} )
ADD_DEFINITIONS( ${ALL_BOOST_PROGRAM_OPTIONS_CXXFLAGS} )

# *** complete compiler flags for feature X11 ***
SET( ALL_X11_CXXFLAGS ${X11_CXXFLAGS} ${SPACE} ${X11_PKG_CXXFLAGS} )
ADD_DEFINITIONS( ${ALL_X11_CXXFLAGS} )

# *** complete compiler flags for feature GMP ***
SET( ALL_GMP_CXXFLAGS ${GMP_CXXFLAGS} ${SPACE} ${GMP_PKG_CXXFLAGS} )
ADD_DEFINITIONS( ${ALL_GMP_CXXFLAGS} )

# *** complete compiler flags for feature GMPXX ***
SET( ALL_GMPXX_CXXFLAGS ${GMPXX_CXXFLAGS} ${SPACE} ${GMPXX_PKG_CXXFLAGS} )
ADD_DEFINITIONS( ${ALL_GMPXX_CXXFLAGS} )

# *** complete compiler flags for feature MPFR ***
SET( ALL_MPFR_CXXFLAGS ${MPFR_CXXFLAGS} ${SPACE} ${MPFR_PKG_CXXFLAGS} )
ADD_DEFINITIONS( ${ALL_MPFR_CXXFLAGS} )

# *** complete compiler flags for feature ZLIB ***
SET( ALL_ZLIB_CXXFLAGS ${ZLIB_CXXFLAGS} ${SPACE} ${ZLIB_PKG_CXXFLAGS} )
ADD_DEFINITIONS( ${ALL_ZLIB_CXXFLAGS} )

# *** complete compiler flags for feature BLAS ***
SET( ALL_BLAS_CXXFLAGS $(BLAS_CXXFLAGS) ${SPACE} $(REFBLASGCC_PKG_CXXFLAGS) )
ADD_DEFINITIONS( ${ALL_BLAS_CXXFLAGS} )

# *** complete compiler flags for feature LAPACK ***
SET( ALL_LAPACK_CXXFLAGS ${LAPACK_CXXFLAGS} ${SPACE} ${DEFAULTLAPACK_PKG_CXXFLAGS} )
ADD_DEFINITIONS( ${ALL_LAPACK_CXXFLAGS} )

# *** complete compiler flags for feature OPENGL ***
SET( ALL_OPENGL_CXXFLAGS ${OPENGL_CXXFLAGS} ${SPACE} ${OPENGL_PKG_CXXFLAGS} )
ADD_DEFINITIONS( ${ALL_OPENGL_CXXFLAGS} )

# *** to work around the long-name-problem with sun as/ld
SET( LONG_NAME_PROBLEM_CXXFLAGS "" )

IF(UNIX)
SET( CGAL_CXXFLAGS "-Wall -frounding-math" ${SPACE} -I${CGAL_INCL_CONF_DIR} ${SPACE} -I${CGAL_INCL_DIR}
		   ${SPACE} ${CUSTOM_CXXFLAGS}
		   ${SPACE} ${BOOST_CXXFLAGS}
		   ${SPACE} ${BOOST_PROGRAM_OPTIONS_CXXFLAGS}
		   ${SPACE} ${X11_CXXFLAGS}
		   ${SPACE} ${GMP_CXXFLAGS}
		   ${SPACE} ${GMPXX_CXXFLAGS}
		   ${SPACE} ${MPFR_CXXFLAGS}
		   ${SPACE} ${ZLIB_CXXFLAGS}
		   ${SPACE} ${BLAS_CXXFLAGS}
		   ${SPACE} ${LAPACK_CXXFLAGS}
		   ${SPACE} ${OPENGL_CXXFLAGS}
		   ${SPACE} ${BOOST_PKG_CXXFLAGS}
		   ${SPACE} ${BOOST_PROGRAM_OPTIONS_PKG_CXXFLAGS}
		   ${SPACE} ${X11_PKG_CXXFLAGS}
		   ${SPACE} ${GMP_PKG_CXXFLAGS}
		   ${SPACE} ${GMPXX_PKG_CXXFLAGS}
		   ${SPACE} ${MPFR_PKG_CXXFLAGS}
		   ${SPACE} ${ZLIB_PKG_CXXFLAGS}
		   ${SPACE} ${REFBLASGCC_PKG_CXXFLAGS}
		   ${SPACE} ${OPENGL_PKG_CXXFLAGS}
		   ${SPACE} ${DEFAULTLAPACK_PKG_CXXFLAGS} )
ADD_DEFINITIONS( ${CGAL_CXXFLAGS} )
ENDIF(UNIX)

SET( CGAL_WINLIB_CXXFLAGS ${CGAL_LIB_CXXFLAGS} )


#---------------------------------------------------------------------#
#                    linker flags
#---------------------------------------------------------------------#

# *** Flag to define the name of the produced executable ***
#EXE_OPT = -o $(SAVESTHEBLANK)

# *** libpath flags for feature BOOST ***
SET( BOOST_LIBPATHFLAGS "" )

# *** libpath flags for feature BOOST_PROGRAM_OPTIONS ***
SET( BOOST_PROGRAM_OPTIONS_LIBPATHFLAGS "" )

# *** libpath flags for feature X11 ***
SET( X11_LIBPATHFLAGS "" )

# *** libpath flags for feature GMP ***
SET( GMP_LIBPATHFLAGS "" )

# *** libpath flags for feature GMPXX ***
SET( GMPXX_LIBPATHFLAGS "" )

# *** libpath flags for feature MPFR ***
SET( MPFR_LIBPATHFLAGS "" )

# *** libpath flags for feature ZLIB ***
SET( ZLIB_LIBPATHFLAGS "" )

# *** libpath flags for feature BLAS ***
SET( BLAS_LIBPATHFLAGS "" )

# *** libpath flags for feature LAPACK ***
SET( LAPACK_LIBPATHFLAGS "" )

# *** libpath flags for feature OPENGL ***
SET( OPENGL_LIBPATHFLAGS "" )

# *** libpath flags for CGAL ***
SET( CGAL_LIBPATHFLAGS -L${CGAL_LIB_DIR}/${CGAL_OS_COMPILER} ${SPACE} ${BOOST_LIBPATHFLAGS}
	${SPACE} ${BOOST_PROGRAM_OPTIONS_LIBPATHFLAGS}
	${SPACE} ${X11_LIBPATHFLAGS}
	${SPACE} ${GMP_LIBPATHFLAGS}
	${SPACE} ${GMPXX_LIBPATHFLAGS}
	${SPACE} ${MPFR_LIBPATHFLAGS}
	${SPACE} ${ZLIB_LIBPATHFLAGS}
	${SPACE} ${BLAS_LIBPATHFLAGS}
	${SPACE} ${LAPACK_LIBPATHFLAGS}
	${SPACE} ${OPENGL_LIBPATHFLAGS}
	${SPACE} "-Wl" ${SPACE} -R${CGAL_LIB_DIR}/${CGAL_OS_COMPILER} )

# *** Fill in any additional linker flags you need ***
SET( CUSTOM_LDFLAGS "" )

# *** to work around the long-name-problem with sun as/ld
SET( LONG_NAME_PROBLEM_LDFLAGS "" )

SET( CGAL_LIBPATH "" )

SET( CGAL_LD_LIBPATH ${CGAL_LIBPATHFLAGS} )

# *** linker flags for package BOOST ***
SET( BOOST_PKG_LDFLAGS "" )

# *** linker flags for package BOOST_PROGRAM_OPTIONS ***
# BOOST_PROGRAM_OPTIONS_PKG_LDFLAGS =  \
# -lboost_program_options 

# *** linker flags for package X11 ***
SET( X11_PKG_LDFLAGS -lX11 )

# *** linker flags for package GMP ***
SET( GMP_PKG_LDFLAGS -lgmp )

# *** linker flags for package GMPXX ***
SET( GMPXX_PKG_LDFLAGS -lgmpxx )

# *** linker flags for package MPFR ***
SET( MPFR_PKG_LDFLAGS -lmpfr )

# *** linker flags for package ZLIB ***
SET( ZLIB_PKG_LDFLAGS -lz )

# *** linker flags for package REFBLASGCC ***
SET( REFBLASGCC_PKG_LDFLAGS -lblas ${SPACE} -lg2c )

# *** linker flags for package OPENGL ***
SET( OPENGL_PKG_LDFLAGS -lGL ${SPACE} -lGLU )

# *** linker flags for package DEFAULTLAPACK ***
SET( DEFAULTLAPACK_PKG_LDFLAGS -llapack )

# *** linker flags for CGAL ***
SET( CGAL_CGAL_LDFLAGS -lCGAL )

SET( CGAL_LIBRARIES -lCGALcore++ ${SPACE} -lCGALimageIO ${SPACE} -lCGALPDB )

SET( CGAL_LDFLAGS ${CGAL_LD_LIBPATH}
		${SPACE} ${CUSTOM_LDFLAGS}
		${SPACE} ${CGAL_CGAL_LDFLAGS}
		${SPACE} -lCGALcore++
		${SPACE} -lCGALimageIO
		${SPACE} -lCGALPDB
		${SPACE} ${DEFAULTLAPACK_PKG_LDFLAGS}
		${SPACE} ${OPENGL_PKG_LDFLAGS}
		${SPACE} ${REFBLASGCC_PKG_LDFLAGS}
		${SPACE} ${ZLIB_PKG_LDFLAGS}
		${SPACE} ${MPFR_PKG_LDFLAGS}
		${SPACE} ${GMPXX_PKG_LDFLAGS}
		${SPACE} ${GMP_PKG_LDFLAGS}
		${SPACE} ${X11_PKG_LDFLAGS}
		${SPACE} ${BOOST_PROGRAM_OPTIONS_PKG_LDFLAGS}
		${SPACE} ${BOOST_PKG_LDFLAGS}
		${SPACE} -lm )

# *** linker flags for feature BOOST ***
SET( BOOST_LDFLAGS ${BOOST_PKG_LDFLAGS} )

# *** linker flags for feature BOOST_PROGRAM_OPTIONS ***
SET( BOOST_PROGRAM_OPTIONS_LDFLAGS ${BOOST_PROGRAM_OPTIONS_PKG_LDFLAGS} )

# *** linker flags for feature X11 ***
SET( X11_LDFLAGS ${X11_PKG_LDFLAGS} )

# *** linker flags for feature GMP ***
SET( GMP_LDFLAGS ${GMP_PKG_LDFLAGS} )

# *** linker flags for feature GMPXX ***
SET( GMPXX_LDFLAGS ${GMPXX_PKG_LDFLAGS} )

# *** linker flags for feature MPFR ***
SET( MPFR_LDFLAGS ${MPFR_PKG_LDFLAGS} )

# *** linker flags for feature ZLIB ***
SET( ZLIB_LDFLAGS ${ZLIB_PKG_LDFLAGS} )

# *** linker flags for feature BLAS ***
SET( BLAS_LDFLAGS ${REFBLASGCC_PKG_LDFLAGS} )

# *** linker flags for feature LAPACK ***
SET( LAPACK_LDFLAGS ${DEFAULTLAPACK_PKG_LDFLAGS} )

# *** linker flags for feature OPENGL ***
SET( OPENGL_LDFLAGS ${OPENGL_PKG_LDFLAGS} )

# *** Qt meta object compiler executable ***
#     (No Qt; therefore, we fake it...)
# QT_MOC = echo dummymoc


#---------------------------------------------------------------------#
#                    commands and flags for creating libraries
#---------------------------------------------------------------------#

#SET( CGAL_STATIC_LIBNAME lib${LIB_NAME_BASE}.a )
#SET( CGAL_SHARED_LIBNAME lib${LIB_NAME_BASE}${CGAL_SHARED_LIB_EXT} )
#SET( CGAL_LIB_CXXFLAGS = "-O2 -fno-strict-aliasing" ${SPACE} ${CGAL_CXXFLAGS} )
# CGAL_LIB_LDFLAGS = 
#
#CGAL_STATIC_LIB_CREATE = ar cr ''
#CGAL_OBJ_PREFIX = 
#CGAL_LIB_DESTINATION = '$(CGAL_LIB_DIR)/$(CGAL_OS_COMPILER)'
#CGAL_SHARED_LIB_CXXFLAGS = \
#	-fPIC -O2 -fno-strict-aliasing \
#	$(CGAL_CXXFLAGS)
#CGAL_SHARED_LIB_LDFLAGS = $(CGAL_LIBPATHFLAGS) $(CUSTOM_LDFLAGS) 
#CGAL_SHARED_LIB_CREATE = $(CGAL_CXX) -shared  -o ''
#
#CGAL_SHARED_LIB_SONAME = -Wl,-soname,$(CGAL_SHARED_LIBNAME).$(SOMAJOR)
#CGAL_SHARED_LIBNAME_WITH_SOMAJOR = $(CGAL_SHARED_LIBNAME).$(SOMAJOR)
#CGAL_SHARED_LIBNAME_WITH_SOVERSION = $(CGAL_SHARED_LIBNAME).$(SOVERSION)
