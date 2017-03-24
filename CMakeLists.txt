cmake_minimum_required(VERSION 3.6)
project(em_core)

set(CMAKE_CXX_STANDARD 11)

set(PROJECT_PATH  "${CMAKE_CURRENT_SOURCE_DIR}")
set(INCLUDE_PATH  "${PROJECT_PATH}/include")
set(SRC_PATH      "${PROJECT_PATH}/src")
set(TEST_SRC_PATH "${PROJECT_PATH}/test")

include_directories(AFTER "${INCLUDE_PATH}")

set(HEADER_FILES )

file(GLOB HEADER_FILES "${INCLUDE_PATH}/*/*.h")
file(GLOB SOURCE_FILES "${SRC_PATH}/*/*.cpp")

#set(SOURCE_FILES  src/base/type.cpp src/base/object.cpp src/os/timer.cpp src/base/array.cpp)
add_executable(em_core src/main.cpp ${SOURCE_FILES} ${HEADER_FILES})