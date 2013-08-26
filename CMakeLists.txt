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
set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -m64" )
set (LIBS ${LIBS} ${CFITSIO}/lib/libcfitsio.a)
set(SRCDIR "src/")
set(INCLUDEDIR "src/")
include_directories( ${INCLUDEDIR} )
include_directories( ${CFITSIO}/include )
set(SOURCES ${SRCDIR}IO/InputFileFITS.cpp
			${SRCDIR}IO/OutputFileFITS.cpp
			${SRCDIR}IO/InputFileText.cpp)
add_library(${LIBNAME} SHARED ${SOURCES})
target_link_libraries(${LIBNAME} ${LIBS})

# build the tests
set (LIBS ${LIBS} ${LIBNAME})
set (LIBS ${LIBS} boost_unit_test_framework)
add_executable(testFileFITS ${SRCDIR}/IO/tests/testFileFITS.cpp)
target_link_libraries(testFileFITS ${LIBS})

enable_testing()
add_test(testFileFITS testFileFITS)

# this copy is 2 lines with cmake 2.8..
set(TESTFILES img.csv sample.fits)
foreach(testfile ${TESTFILES})
add_custom_command(TARGET testFileFITS
                   POST_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy_if_different
                   ${CMAKE_CURRENT_SOURCE_DIR}/src/IO/tests/${testfile}
                   ${CMAKE_CURRENT_BINARY_DIR}/${testfile})
                   set(RES ${RES} ${CMAKE_CURRENT_BINARY_DIR}/${testfile})
endforeach(testfile)
add_custom_command(SOURCE testFileFITS TARGET testFileFITS DEPENDS ${res})

add_custom_command(TARGET testFileFITS POST_BUILD COMMAND testFileFITS)

# make docs
FIND_PACKAGE(Doxygen)
if (NOT DOXYGEN_FOUND)
  message(FATAL_ERROR
    "Doxygen is needed to build the documentation. Please install it correctly")
endif()
configure_file(Doxyfile.in ${PROJECT_BINARY_DIR}/Doxyfile @ONLY IMMEDIATE)
add_custom_target(docs COMMAND ${DOXYGEN_EXECUTABLE} ${PROJECT_BINARY_DIR}/Doxyfile)

# make install
file(GLOB HEADERS "${INCLUDEDIR}/IO/*.h")
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
