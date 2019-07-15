cmake_minimum_required(VERSION 3.0.0)
project(common VERSION 0.1.0)
set(CMAKE_CXX_STANDARD 17)
include_directories(include)

add_library(common src/observer.cpp include/observer.hpp)
ENABLE_TESTING()
add_subdirectory(common_test)