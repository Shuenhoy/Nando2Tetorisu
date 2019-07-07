cmake_minimum_required(VERSION 3.0.0)
project(Model VERSION 0.1.0)
set(CMAKE_CXX_STANDARD 17)


add_library(Model src/model.cpp include/model.cpp)
ENABLE_TESTING()
add_subdirectory(test)