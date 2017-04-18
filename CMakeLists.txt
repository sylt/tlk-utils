cmake_minimum_required (VERSION 3.0)

project (tlk-utils)

file(GLOB_RECURSE lib_sources "lib/*.cpp")

add_executable(tlkview ${lib_sources} utils/tlkview.cpp)
target_include_directories(tlkview PUBLIC lib)

add_executable(tlkcombine ${lib_sources} utils/tlkcombine.cpp)
target_include_directories(tlkcombine PUBLIC lib)

add_executable(tlkreplace ${lib_sources} utils/tlkreplace.cpp)
target_include_directories(tlkreplace PUBLIC lib)
