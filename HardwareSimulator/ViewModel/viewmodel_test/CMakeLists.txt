set(CMAKE_CXX_STANDARD 17)
add_executable(VIEWMODELTEST test.cpp)
target_link_libraries(VIEWMODELTEST Model ViewModel)
