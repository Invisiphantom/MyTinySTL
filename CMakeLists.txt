cmake_minimum_required(VERSION 3.22)

project(MyTinySTL)

set(CMAKE_BUILD_TYPE release)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2 -Wall -Wextra -Wno-sign-compare -Wno-unused-but-set-variable -Wno-array-bounds")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++23")
message(STATUS "The cmake_cxx_flags is: ${CMAKE_CXX_FLAGS}")

add_subdirectory(${PROJECT_SOURCE_DIR}/Test)
