cmake_minimum_required(VERSION 2.8 FATAL_ERROR)

project(QLBase)
set(QLBase_MAJOR_VERSION 0)
set(QLBase_MINOR_VERSION 3)
set(QLBase_PATCH_VERSION 3)
set(QLBase_VERSION
${QLBase_MAJOR_VERSION}.${QLBase_MINOR_VERSION}.${QLBase_PATCH_VERSION})
set(PROJECT_VERSION ${QLBase_VERSION})

set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake-modules)

add_subdirectory(code)
add_subdirectory(scripts)

find_package(Boost COMPONENTS system filesystem unit_test_framework)
if(Boost_FOUND)
    add_subdirectory(tests)
    enable_testing()
    add_test(testFileFITS testFileFITS)
    add_test(testFileText testFileText)
endif(Boost_FOUND)
