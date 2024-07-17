cmake_minimum_required(VERSION 3.30)
project(Pong)

add_subdirectory(raylib)

set(CMAKE_CXX_STANDARD 23)

add_executable(Pong main.cpp)

target_link_libraries(Pong PRIVATE raylib)
