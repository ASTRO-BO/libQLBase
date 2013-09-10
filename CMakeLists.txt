cmake_minimum_required(VERSION 2.8 FATAL_ERROR)

project(QLBase)
set(QLBase_MAJOR_VERSION 0)
set(QLBase_MINOR_VERSION 1)
set(QLBase_PATCH_VERSION 0)

set(CMAKE_MODULE_PATH ${QLBase_SOURCE_DIR}/cmake-modules)

add_subdirectory(code)
add_subdirectory(tests)

enable_testing()
add_test(testFileFITS testFileFITS)
add_test(testFileText testFileText)
