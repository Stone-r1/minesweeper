cmake_minimum_required(VERSION 3.12)
project(minesweeper CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(raylib REQUIRED)

set(SOURCES
    src/main.cpp
    src/cell.cpp
    src/board.cpp
    src/statusbar.cpp
)

add_executable(${PROJECT_NAME} ${SOURCES})

target_include_directories(${PROJECT_NAME} PRIVATE src)
target_link_libraries(${PROJECT_NAME} raylib)
