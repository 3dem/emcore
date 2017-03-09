cmake_minimum_required(VERSION 3.6)
project(em_core)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES main.cpp type.h type.cpp object.cpp object.h os/timer.cpp os/timer.h array.cpp array.h)
add_executable(em_core ${SOURCE_FILES})