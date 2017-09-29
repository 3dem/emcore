cmake_minimum_required(VERSION 3.0)
project(em-core)

# Add option to build examples, turn on with 'cmake -DBUILD_TESTS=ON'
option(BUILD_EXAMPLES "Build all examples." OFF)
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


find_package(TIFF)
if (TIFF_FOUND)
    include_directories(${TIFF_INCLUDE_DIRS})
    MESSAGE( STATUS "TIFF_INCLUDE_DIRS:         " ${TIFF_INCLUDE_DIRS} )
    MESSAGE( STATUS "TIFF_LIBRARIES:         " ${TIFF_LIBRARIES} )
else (TIFF_FOUND)
    list(REMOVE_ITEM SOURCE_FILES "${SRC_PATH}/image/image_tiff.cpp")
endif (TIFF_FOUND)

find_package(JPEG)
find_package(PNG)

add_library(em-core SHARED ${SOURCE_FILES} ${HEADER_FILES} ${TIFF_LIBRARIES})


################################
# Examples
################################
if (BUILD_EXAMPLES)
    #add_subdirectory(examples)

    file(GLOB EXAMPLE_FILES "${CMAKE_SOURCE_DIR}/examples/*.cpp")

    foreach (_file ${EXAMPLE_FILES})
        # Specify target name WE=WithoutExtension
        GET_FILENAME_COMPONENT(_target "${_file}" NAME_WE)
        message(STATUS "File ${_file}...")
        message(STATUS "target ${_target}...")

        add_executable(${_target} ${_file})
        target_link_libraries(${_target} em-core ${TIFF_LIBRARIES})

    endforeach()




    #add_executable(example examples/example.cpp)
    #target_link_libraries(example em-core)

endif()


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
    add_executable(em-test-base test/test_base.cpp)
    add_executable(em-test-image test/test_image.cpp)

    # Standard linking to gtest stuff.
    target_link_libraries(em-test gtest gtest_main ${TIFF_LIBRARIES})
    target_link_libraries(em-test-base gtest gtest_main ${TIFF_LIBRARIES})
    target_link_libraries(em-test-image gtest gtest_main ${TIFF_LIBRARIES})

    # Extra linking for the project.
    target_link_libraries(em-test em-core)
    target_link_libraries(em-test-base em-core)
    target_link_libraries(em-test-image em-core)

    # This is so you can do 'make test' to see all your tests run, instead of
    # manually running the executable em-test to see those specific tests.
    add_test(em-test em-test)
    add_test(em-test-base em-test-base)
    add_test(em-test-image em-test-image)


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

