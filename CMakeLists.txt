cmake_minimum_required(VERSION 3.16)
project(SerialTools)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(ftxui REQUIRED)

add_executable(SerialTools
    src/main.cpp
    src/mainwindow.cpp
    src/tty_device/serial.c
    src/tty_device/mypthread.c
)

target_link_libraries(SerialTools 
    PRIVATE ftxui::screen
    PRIVATE ftxui::dom
    PRIVATE ftxui::component
    PRIVATE pthread
)