#include doxygen macro
include(MatisDoxygenMacros)
set(CMAKE_VERBOSITY 1)

#config documentation
OPTION(CUSTOM_BUILD "if set, you could access documentation options " OFF)
__config_documentation()

#set up src using by doxygen documentation
set(LIBMPP_HPP_DIR ${ROOT_DIR}/lib/libRJMCMC/core)
SET(DOXY_EXTRACT_ALL "YES")
SET(DOXY_COLS_IN_ALPHA_INDEX "2")
	
file(GLOB LIBMPP_DOC_SRC ${LIBMPP_HPP_DIR}/*.hpp)
__add_documentation(LibMPP LIBMPP_DOC_SRC)
