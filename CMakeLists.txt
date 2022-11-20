cmake_minimum_required(VERSION 3.0.0)
project(stdset)

set(CMAKE_CXX_FLAGS "-std=c++2a -Wall -Wpedantic -Wextra -ftest-coverage -fPIC -O0")
set(CMAKE_EXE_LINKER_FLAGS "--coverage -lgcov")
add_executable(${PROJECT_NAME} main.cpp)

add_subdirectory(set)

target_link_libraries(${PROJECT_NAME} PUBLIC set)

add_subdirectory(tests)