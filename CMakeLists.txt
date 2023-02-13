cmake_minimum_required(VERSION 3.22.1)

project(number-theory LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)

# add_library(project_options INTERFACE)

add_library(ivl INTERFACE)
target_include_directories(ivl INTERFACE src)

add_executable(hello bin/hello.cpp)
target_link_libraries(hello PRIVATE ivl)
