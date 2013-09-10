set( QLBase_MAJOR_VERSION 0 )
set( QLBase_MINOR_VERSION 1 )
set(LIBNAME ${PROJECT_NAME}.${QLBase_MAJOR_VERSION}.${QLBase_MINOR_VERSION})

set(CMAKE_MODULE_PATH ${QLBase_SOURCE_DIR}/cmake-modules)
message("${QLBase_SOURCE_DIR}")
find_package(CFITSIO REQUIRED)
include_directories(${CFITSIO_INCLUDE_DIR})
set(LIBS ${LIBS} ${CFITSIO_LIBRARIES})

set(SOURCES IO/InputFileFITS.cpp
			IO/OutputFileFITS.cpp
			IO/InputFileText.cpp)
add_library(${LIBNAME} SHARED ${SOURCES})

# make install
file(GLOB HEADERS "${QLBase_SOURCE_DIR}/src/IO/*.h")
install(FILES ${HEADERS} DESTINATION include)
install(TARGETS ${LIBNAME} DESTINATION lib)

# make uninstall
configure_file(
   "${CMAKE_CURRENT_SOURCE_DIR}/cmake_uninstall.cmake.in"
   "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
    IMMEDIATE @ONLY)
add_custom_target(uninstall
    COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake)

# make docs
find_package(Doxygen)
if (NOT DOXYGEN_FOUND)
  message("Doxygen not found. Cannot generate make docs target.")
endif()
configure_file(../Doxyfile.in ${PROJECT_BINARY_DIR}/Doxyfile @ONLY IMMEDIATE)
add_custom_target(docs COMMAND ${DOXYGEN_EXECUTABLE} ${PROJECT_BINARY_DIR}/Doxyfile)
