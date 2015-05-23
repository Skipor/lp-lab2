cmake_minimum_required(VERSION 3.2)
project(lp_lab2)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES main.cpp node.h parser.hpp tokens.cpp parser.cpp )
add_executable(lp_lab2 ${SOURCE_FILES} node.cpp)