cmake_minimum_required(VERSION 2.6 FATAL_ERROR)

project(astro)

# name the lib with version
set( QL_MAJOR_VERSION 5 )
set( QL_MINOR_VERSION 0 )
set(LIBNAME ${CMAKE_PROJECT_NAME})

set(CFITSIO $ENV{CFITSIO})
if(NOT CFITSIO)
        message(SEND_ERROR "The CFITSIO enviromental variable should be set.")
endif()

set(ROOTSYS $ENV{ROOTSYS})
if(NOT ROOTSYS)
        message(FATAL_ERROR "The ROOTSYS enviromental variable should be set.")
endif()

set(INSTALLDIR $ENV{LIBASTRO_INSTALLDIR})
if(NOT INSTALLDIR)
        message(FATAL_ERROR "The global LIBASTRO_INSTALLDIR enviromental variable should be set.")
endif()

# build the lib
set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -m64" )
include_directories( ${CMAKE_CURRENT_SOURCE_DIR} )
include_directories( ${CFITSIO}/include )
include_directories( ${ROOTSYS}/include/root/ )
include_directories( ${INSTALLDIR}/include )
file(GLOB_RECURSE SOURCES "*.cpp")
file(GLOB_RECURSE SOURCESCXX "*.cxx")
file(GLOB_RECURSE SOURCESC "*.c")
list(APPEND SOURCES TFcoldict.cxx TFdict.cxx TFimgdict.cxx TFstrdict.cxx imagedict.cxx ${SOURCESCXX} ${SOURCESC})
add_library(${LIBNAME} SHARED ${SOURCES} )

# call rootcint to generate QLDict.cpp
string(REPLACE " " ";" CXXINCPATH ${CMAKE_CXX_FLAGS})

set(IMAGEDICT astroImage.h astroRegion.h imageLinkDef.h)
set(ROOTCINT_PARAMS -f imagedict.cxx -c -I${CMAKE_CURRENT_SOURCE_DIR}
${CXXINCPATH} -I${CFITSIO}/include -I${LIBASTRO_INSTALLDIR}/include ${IMAGEDICT})
add_custom_command(OUTPUT imagedict.h imagedict.cxx COMMAND rootcint ${ROOTCINT_PARAMS})

set(TFCOLDICT TFColumn.h TFGroup.h TFColLinkDef.h)
set(ROOTCINT_PARAMS -f TFcoldict.cxx -c -I${CMAKE_CURRENT_SOURCE_DIR}
${CXXINCPATH} -I${LIBASTRO_INSTALLDIR}/include ${TFCOLDICT})
add_custom_command(OUTPUT TFcoldict.h TFcoldict.cxx COMMAND rootcint ${ROOTCINT_PARAMS})

set(TFDICT TFError.h TFHeader.h TFIOElement.h TFRootIO.h TFFitsIO.h TFTable.h TFGroup.h TFRowIterator.h TFNameConvert.h TFLinkDef.h)
set(ROOTCINT_PARAMS -f TFdict.cxx -c -I${CMAKE_CURRENT_SOURCE_DIR}
${CXXINCPATH} -I${LIBASTRO_INSTALLDIR}/include ${TFDICT})
add_custom_command(OUTPUT TFdict.h TFdict.cxx COMMAND rootcint ${ROOTCINT_PARAMS})

set(TFIMGDICT TFImage.h TFImgLinkDef.h)
set(ROOTCINT_PARAMS -f TFimgdict.cxx -c -I${CMAKE_CURRENT_SOURCE_DIR}
${CXXINCPATH} -I${LIBASTRO_INSTALLDIR}/include ${TFIMGDICT})
add_custom_command(OUTPUT TFimgdict.h TFimgdict.cxx COMMAND rootcint ${ROOTCINT_PARAMS})

set(TFSTRDICT TFTStringLinkDef.h)
set(ROOTCINT_PARAMS -f TFstrdict.cxx -c -I${CMAKE_CURRENT_SOURCE_DIR}
${CXXINCPATH} -I${LIBASTRO_INSTALLDIR}/include ${TFSTRDICT})
add_custom_command(OUTPUT TFstrdict.h TFstrdict.cxx COMMAND rootcint ${ROOTCINT_PARAMS})

# make install
file(GLOB HEADERS "*.h")
install(FILES ${HEADERS} DESTINATION include)
install(TARGETS ${LIBNAME} DESTINATION lib)

# make uninstall
configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake_uninstall.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
    IMMEDIATE @ONLY)

add_custom_target(uninstall
    COMMAND ${CMAKE_COMMAND} -P
${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake)
