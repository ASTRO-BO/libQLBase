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
