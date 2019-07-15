cmake_minimum_required(VERSION 3.0.0)
project(Model VERSION 0.1.0)
set(CMAKE_CXX_STANDARD 17)
include_directories(include)
include_directories(deps)
add_library(Model src/model.cpp include/model.hpp include/script.hpp src/script.cpp src/chip.cpp include/chip.hpp include/executor.hpp include/builder.hpp src/builder.cpp src/executor.cpp)

ENABLE_TESTING()
add_subdirectory(test)