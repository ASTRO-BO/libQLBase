cmake_minimum_required(VERSION 2.6 FATAL_ERROR)

project(QLBase)

set(CFITSIO $ENV{CFITSIO})
if(NOT CFITSIO)
	message(SEND_ERROR "The CFITSIO enviromental variable should be set.")
endif()

# lib the name with version
set( QLBase_MAJOR_VERSION 0 )
set( QLBase_MINOR_VERSION 1 )
set(LIBNAME ${CMAKE_PROJECT_NAME}.${QLBase_MAJOR_VERSION}.${QLBase_MINOR_VERSION})

# build the lib
set(SRCDIR "src/")
set(INCLUDEDIR "src/")
include_directories( ${INCLUDEDIR} )
include_directories( ${CFITSIO}/include )
set(SOURCES ${SRCDIR}IO/InputFileFITS.cpp
			${SRCDIR}IO/OutputFileFITS.cpp
			${SRCDIR}IO/InputFileText.cpp)
add_library(${LIBNAME} SHARED ${SOURCES})
target_link_libraries(${LIBNAME} ${CFITSIO}/lib/libcfitsio.a)

# make docs
FIND_PACKAGE(Doxygen)
if (NOT DOXYGEN_FOUND)
  message(FATAL_ERROR
    "Doxygen is needed to build the documentation. Please install it correctly")
endif()
configure_file(Doxyfile.in ${PROJECT_BINARY_DIR}/Doxyfile @ONLY IMMEDIATE)
add_custom_target(docs COMMAND ${DOXYGEN_EXECUTABLE} ${PROJECT_BINARY_DIR}/Doxyfile)

# make install
install(DIRECTORY ${INCLUDEDIR} DESTINATION include
		FILES_MATCHING PATTERN "*.h"
		PATTERN "tests" EXCLUDE)
install(TARGETS ${LIBNAME} DESTINATION lib)

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wall -pedantic")
# make uninstall
configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake_uninstall.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
    IMMEDIATE @ONLY)

add_custom_target(uninstall
    COMMAND ${CMAKE_COMMAND} -P
${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake)
