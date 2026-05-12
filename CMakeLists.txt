cmake_minimum_required(VERSION 3.16)
project(SerialTools)

# include(FetchContent)
# FetchContent_Declare(ftxui
#     GIT_REPOSITORY https://github.com/ArthurSonzogni/FTXUI
#     GIT_TAG v6.1.9
# )

# FetchContent_MakeAvailable(ftxui)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)

# find_package(Qt6 REQUIRED COMPONENTS Core Widgets)

find_package(ftxui REQUIRED)

add_executable(SerialTools
    src/main.cpp
    src/mainwindow.cpp
    src/mainwindow.hpp
)

target_link_libraries(SerialTools 
    PRIVATE ftxui::screen
    PRIVATE ftxui::dom
    PRIVATE ftxui::component

)



