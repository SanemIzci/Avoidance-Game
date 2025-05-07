cmake_minimum_required(VERSION 3.0)
project(raylib_game)

set(CMAKE_CXX_STANDARD 17)

include_directories(C:/raylib/include)
link_directories(C:/raylib/lib)

add_executable(raylib_game main.cpp)
target_link_libraries(raylib_game raylib opengl32 gdi32 winmm)
