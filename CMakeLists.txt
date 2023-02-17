cmake_minimum_required(VERSION 3.22.1)

project(number-theory LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
add_compile_options(-Wall -Wextra -Wshadow -Wnon-virtual-dtor -pedantic)
if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
  add_compile_options(-fimplicit-constexpr) # everything is constexpr :D
  add_compile_options(-fconcepts-diagnostics-depth=3) # damn concept errors are somehow worse than tmp errors
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
  add_compile_options(-Weverything -Wno-c++98-compat -Wno-weak-vtables -Wno-zero-as-null-pointer-constant -Wno-padded)
endif()

include_directories(src)

add_executable(hello bin/hello.cpp)

add_executable(hello2 bin/hello2.cpp)

add_executable(hello3 bin/hello3.cpp)
