cmake_minimum_required(VERSION 3.1.0)
project(Nando2Tetorisu VERSION 0.1.0)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
find_package(Qt5 COMPONENTS Core Gui Widgets REQUIRED)
include_directories(../common/include
    View/include
    ViewModel/include
    Model/include)

add_subdirectory(View View.out)
add_executable(${PROJECT_NAME} main.cpp)
target_link_libraries(${PROJECT_NAME} Qt5::Core Qt5::Gui Qt5::Widgets View)
ENABLE_TESTING()
