cmake_minimum_required(VERSION 2.6 FATAL_ERROR)

project(QLBase)

# lib the name with version
set( QLBase_MAJOR_VERSION 0 )
set( QLBase_MINOR_VERSION 1 )
set(LIBNAME ${CMAKE_PROJECT_NAME}.${QLBase_MAJOR_VERSION}.${QLBase_MINOR_VERSION})

# build the lib
set(SOURCES "IO/InputFileFITS.cpp" "IO/OutputFileFITS.cpp" "IO/InputFileText.cpp")
add_library(${LIBNAME} SHARED ${SOURCES})

# make docs
FIND_PACKAGE(Doxygen)
if (NOT DOXYGEN_FOUND)
  message(FATAL_ERROR
    "Doxygen is needed to build the documentation. Please install it correctly")
endif()
configure_file(../Doxyfile.in ${PROJECT_BINARY_DIR}/Doxyfile @ONLY IMMEDIATE)
add_custom_target(docs COMMAND ${DOXYGEN_EXECUTABLE} ${PROJECT_BINARY_DIR}/Doxyfile)

# unix /usr/local installation
install(DIRECTORY IO/ DESTINATION include/QL/IO
            FILES_MATCHING PATTERN "*.h")
install(TARGETS ${LIBNAME} DESTINATION lib64)
install(DIRECTORY ${PROJECT_BINARY_DIR}/doc/ DESTINATION share/doc/${CMAKE_PROJECT_NAME})

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wall -pedantic")
