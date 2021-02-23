cmake_minimum_required(VERSION 3.16)
project(3SH3)

set(CMAKE_CXX_STANDARD 20)

file(GLOB LAB1 "Lab 1/*.h" "Lab 1/*.cpp")
file(GLOB LAB2 "Lab 2/*.h" "Lab 2/main.cpp")
file(GLOB LAB3 "Lab 3/*.h" "Lab 3/*.cpp")
file(GLOB LAB4 "Lab 4/*.h" "Lab 4/*.cpp")
file(GLOB LAB5 "Lab 5/*.h" "Lab 5/*.cpp")

add_executable(3SH3 ${LAB2})