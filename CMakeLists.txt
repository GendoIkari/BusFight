cmake_minimum_required(VERSION 3.5)

project(BusFight LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 COMPONENTS Widgets Svg REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Widgets REQUIRED)

set(PROJECT_SOURCES
        main.cpp
        mainwindow.cpp
        mainwindow.h
        sectionwidget.cpp
        sectionwidget.h
        fieldsdialog.cpp
        fieldsdialog.h
        sectiondialog.cpp
        sectiondialog.h
        componentsdock.cpp
        componentsdock.h
        entities.cpp
        entities.h
)

add_executable(BusFight
    ${PROJECT_SOURCES} res.qrc
)

target_link_libraries(BusFight PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)
