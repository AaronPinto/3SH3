cmake_minimum_required(VERSION 3.16)
project(3SH3)

set(CMAKE_CXX_STANDARD 20)

file(GLOB LAB1 "Lab 1/*.h" "Lab 1/*.cpp")

add_executable(3SH3 ${LAB1})