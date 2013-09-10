cmake_minimum_required(VERSION 2.8 FATAL_ERROR)

project(QLBase)

add_subdirectory(src)
add_subdirectory(src/IO/tests)

enable_testing()
add_test(testFileFITS testFileFITS)
add_test(testFileText testFileText)

# make docs
#FIND_PACKAGE(Doxygen)
#if (NOT DOXYGEN_FOUND)
#  message(FATAL_ERROR
#    "Doxygen is needed to build the documentation. Please install it correctly")
#endif()
#configure_file(Doxyfile.in ${PROJECT_BINARY_DIR}/Doxyfile @ONLY IMMEDIATE)
#add_custom_target(docs COMMAND ${DOXYGEN_EXECUTABLE} ${PROJECT_BINARY_DIR}/Doxyfile)

#add_custom_target(uninstall
#    COMMAND ${CMAKE_COMMAND} -P
#${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake)
