set(CMAKE_CXX_STANDARD 17)
add_executable(commontest test.cpp)
target_link_libraries(commontest common)
