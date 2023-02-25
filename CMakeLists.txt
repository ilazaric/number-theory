cmake_minimum_required(VERSION 3.22.1)

project(number-theory LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
add_compile_options(-Wall -Wextra -Wshadow -Wnon-virtual-dtor -pedantic -Werror)
if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
  add_compile_options(-fimplicit-constexpr) # everything is constexpr :D
  add_compile_options(-fconcepts-diagnostics-depth=3) # damn concept errors are somehow worse than tmp errors
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
  # removed Wpadded bc of course padding happens
  # removed Wc++98-compat bc nobody cares
  # removed Wzero-as-null-pointer-constant bc its broken with the spaceship (<=>) operator
  # don't quite remember why i removed Wweak-vtables
  # * it yells at me about custom exceptions, dont see the issue atm
  # removed Wc++98-compat-pedantic bc nobody cares
  add_compile_options(-Weverything -Wno-c++98-compat -Wno-weak-vtables -Wno-zero-as-null-pointer-constant -Wno-padded -Wno-c++98-compat-pedantic)
endif()

include_directories(src)

add_executable(hello bin/hello.cpp)
add_executable(hello2 bin/hello2.cpp)
add_executable(hello3 bin/hello3.cpp)

enable_testing()
add_executable(Test test/test.cpp)
target_compile_options(Test PRIVATE -fsanitize=address)
target_link_options(Test PRIVATE -fsanitize=address)
target_compile_options(Test PRIVATE -fsanitize=undefined)
target_link_options(Test PRIVATE -fsanitize=undefined)
add_test(NAME MyTest COMMAND Test)

