cmake_minimum_required(VERSION 3.6)
project(em-core)

# Add option to build tests, turn on with 'cmake -DBUILD_TESTS=ON'
option(BUILD_TESTS "Build all tests." OFF)
# Add option to build the doxygen documentation, turn on with 'cmake -DBUILD_DOCS=ON'
option(BUILD_DOCS "Build the doxygen documentation" OFF)


set(CMAKE_CXX_STANDARD 11)

set(${CMAKE_PROJECT_NAME}_VERSION_MAJOR        0)
set(${CMAKE_PROJECT_NAME}_VERSION_MINOR        0)
set(${CMAKE_PROJECT_NAME}_VERSION_PATCH        1)

set(PROJECT_PATH  "${CMAKE_CURRENT_SOURCE_DIR}")
set(INCLUDE_PATH  "${PROJECT_PATH}/include")
set(SRC_PATH      "${PROJECT_PATH}/src")
set(DOC_PATH      "${PROJECT_PATH}/doc")
set(TEST_SRC_PATH "${PROJECT_PATH}/test")
set(BUILD_PATH      "${PROJECT_PATH}/build")

include_directories(AFTER "${INCLUDE_PATH}")

set(HEADER_FILES test/test_main.cpp)

file(GLOB HEADER_FILES "${INCLUDE_PATH}/*/*.h")
file(GLOB SOURCE_FILES "${SRC_PATH}/*/*.cpp")

add_library(em-core  ${SOURCE_FILES} ${HEADER_FILES})

################################
# Testing
################################
if (BUILD_TESTS)
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

################################
# Documentation
################################
if(BUILD_DOCS)
    find_package(Doxygen)

    if(NOT DOXYGEN_FOUND)
        message(FATAL_ERROR
                "Doxygen is needed to build the documentation. Please make sure it is installed.")
    endif(NOT DOXYGEN_FOUND)

    configure_file(${DOC_PATH}/Doxyfile.in
            ${BUILD_PATH}/Doxyfile @ONLY)
    #configure_file(${PROJECT_PATH}/Doxyfile.in
    #        ${PROJECT_BINARY_DIR}/Doxyfile @ONLY IMMEDIATE)

    # Add a custom target to run Doxygen whenever the project is built.
    # If you do NOT want the documentation to be generated EVERY time you build the project
    # then leave out the 'ALL' keyword from the command below.
    add_custom_target(docs COMMAND ${DOXYGEN_EXECUTABLE} ${BUILD_PATH}/Doxyfile)
endif(BUILD_DOCS)