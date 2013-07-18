cmake_minimum_required(VERSION 2.6 FATAL_ERROR)

project(QLBase)

# lib the name with version
set( QLBase_MAJOR_VERSION 0 )
set( QLBase_MINOR_VERSION 1 )
set(LIBNAME ${CMAKE_PROJECT_NAME}.${QLBase_MAJOR_VERSION}.${QLBase_MINOR_VERSION})

# build the lib
file(GLOB_RECURSE SOURCES "*.cpp")
add_library(${LIBNAME} SHARED ${SOURCES})

# unix /usr/local installation
install(DIRECTORY IO/ DESTINATION include/${CMAKE_PROJECT_NAME}
            FILES_MATCHING PATTERN "*.h")
install(TARGETS ${LIBNAME} DESTINATION lib64)
