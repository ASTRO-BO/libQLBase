cmake_minimum_required(VERSION 2.8 FATAL_ERROR)

project(QLBase)

add_subdirectory(src)
add_subdirectory(src/IO/tests)

enable_testing()
add_test(testFileFITS testFileFITS)
add_test(testFileText testFileText)
