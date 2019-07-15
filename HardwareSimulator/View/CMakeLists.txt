cmake_minimum_required(VERSION 3.1.0)
project(View VERSION 0.1.0)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
add_subdirectory(../ViewModel ViewModel.out)
add_library(View
    src/dialog.cpp include/dialog.h
    src/mainwindow.cpp include/mainwindow.h
    src/pinmodel.cpp include/pinmodel.h
    src/pinview.cpp include/pinview.h
    ui/dialog.ui
    ui/mainwindow.ui)
target_link_libraries(${PROJECT_NAME} ViewModel Qt5::Core Qt5::Gui Qt5::Widgets)
ENABLE_TESTING()
