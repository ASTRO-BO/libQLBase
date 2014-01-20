find_package(CFITSIO REQUIRED)
include_directories(${CFITSIO_INCLUDE_DIR})
set(LIBS ${LIBS} ${CFITSIO_LIBRARIES})

set(SOURCES IO/InputFileFITS.cpp
			IO/OutputFileFITS.cpp
			IO/InputFileText.cpp)
add_library(${CMAKE_PROJECT_NAME} SHARED ${SOURCES})
set_target_properties(${CMAKE_PROJECT_NAME} PROPERTIES SOVERSION ${PROJECT_VERSION})
target_link_libraries(${CMAKE_PROJECT_NAME} ${LIBS})

# make install
file(GLOB HEADERS "${PROJECT_SOURCE_DIR}/code/IO/*.h")
install(FILES ${HEADERS} DESTINATION include/qlbase)
install(TARGETS ${CMAKE_PROJECT_NAME} DESTINATION lib)
file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/../doc)
install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/../doc DESTINATION .)

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
