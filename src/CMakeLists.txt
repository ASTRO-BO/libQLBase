cmake_minimum_required(VERSION 2.6 FATAL_ERROR)

project(libQLBase)

file(GLOB_RECURSE sources "*.cpp")

add_library(QLBase SHARED ${sources})
