cmake_minimum_required(VERSION 3.23)
project(stdset)

set(CMAKE_CXX_STANDARD 23)

add_executable(stdset test_set.cpp set.h set.cpp)
