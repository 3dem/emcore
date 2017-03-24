cmake_minimum_required(VERSION 3.6)
project(em_core)

# Options. Turn on with 'cmake -DTEST=ON'.
option(TEST "Build all tests." OFF) # Makes boolean 'test' available.

set(CMAKE_CXX_STANDARD 11)

set(PROJECT_PATH  "${CMAKE_CURRENT_SOURCE_DIR}")
set(INCLUDE_PATH  "${PROJECT_PATH}/include")
set(SRC_PATH      "${PROJECT_PATH}/src")
set(TEST_SRC_PATH "${PROJECT_PATH}/test")

include_directories(AFTER "${INCLUDE_PATH}")

set(HEADER_FILES test/test_main.cpp)

file(GLOB HEADER_FILES "${INCLUDE_PATH}/*/*.h")
file(GLOB SOURCE_FILES "${SRC_PATH}/*/*.cpp")

add_library(em-core  ${SOURCE_FILES} ${HEADER_FILES})

################################
# Testing
################################
if (TEST)
    # This adds another subdirectory, which has 'project(gtest)'.
    add_subdirectory(libs/googletest/googletest)

    enable_testing()

    # Include the gtest library. gtest_SOURCE_DIR is available due to
    # 'project(gtest)' above.
    include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})

    ##############
    # Unit Tests
    ##############
    add_executable(em-test test/test_main.cpp)

    # Standard linking to gtest stuff.
    target_link_libraries(em-test gtest gtest_main)

    # Extra linking for the project.
    target_link_libraries(em-test em-core)

    # This is so you can do 'make test' to see all your tests run, instead of
    # manually running the executable em-test to see those specific tests.
    add_test(main-test em-test)

endif()