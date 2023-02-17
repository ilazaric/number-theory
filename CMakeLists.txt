cmake_minimum_required(VERSION 3.22.1)

project(number-theory LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
add_compile_options(-Wall -Wextra -Wshadow -Wnon-virtual-dtor -pedantic)
add_compile_options(-fimplicit-constexpr) # everything is constexpr :D

add_library(ivl INTERFACE)
target_include_directories(ivl INTERFACE src)

add_executable(hello bin/hello.cpp)
target_link_libraries(hello PRIVATE ivl)

add_executable(hello2 bin/hello2.cpp)
target_link_libraries(hello2 PRIVATE ivl)

add_executable(hello3 bin/hello3.cpp)
target_link_libraries(hello3 PRIVATE ivl)
