cmake_minimum_required(VERSION 3.16)
project(SerialTools)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)

find_package(Qt6 REQUIRED COMPONENTS Core Widgets)

qt_add_executable(SerialTools
    main.cpp
    mainwindow.cpp
    mainwindow.h
)

target_link_libraries(SerialTools PRIVATE Qt6::Core Qt6::Widgets)



