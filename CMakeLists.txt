cmake_minimum_required(VERSION 3.0)
project(em-core VERSION 0.0.1)

#set(${CMAKE_PROJECT_NAME}_VERSION_MAJOR        0)
#set(${CMAKE_PROJECT_NAME}_VERSION_MINOR        0)
#set(${CMAKE_PROJECT_NAME}_VERSION_PATCH        1)


# Add option to build examples, turn on with 'cmake -DBUILD_TESTS=ON'
option(BUILD_EXAMPLES "Build all examples." OFF)
# Add option to build tests, turn on with 'cmake -DBUILD_TESTS=ON'
option(BUILD_TESTS "Build all tests." OFF)
# Add option to build the doxygen documentation, turn on with 'cmake -DBUILD_DOCS=ON'
option(BUILD_DOCS "Build the doxygen documentation" OFF)
# Add option to decide if build tiff or not, useful when the library is not detected
option(BUILD_TIFF "Build library with TIFF support" ON)


set(CMAKE_CXX_STANDARD 11)

add_definitions(-DEM_CORE_VERSION="${PROJECT_VERSION}")

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

set(EXT_LIBRARIES ) # Defined empty to be fullfiled, if necessary

find_package(TIFF)
if (TIFF_FOUND AND BUILD_TIFF)
    include_directories(${TIFF_INCLUDE_DIRS})
    MESSAGE( STATUS "TIFF_INCLUDE_DIRS:         " ${TIFF_INCLUDE_DIRS} )
    MESSAGE( STATUS "TIFF_LIBRARIES:         " ${TIFF_LIBRARIES} )

    set(EXT_LIBRARIES ${EXT_LIBRARIES} ${TIFF_LIBRARIES})

else ()
    list(REMOVE_ITEM SOURCE_FILES "${SRC_PATH}/image/image_tiff.cpp")
endif ()

#find_package(JPEG)
#find_package(PNG)

add_library(em-core SHARED ${SOURCE_FILES} ${HEADER_FILES})
target_link_libraries( em-core ${EXT_LIBRARIES} )


################################
# Examples
################################
if (BUILD_EXAMPLES)
    #add_subdirectory(examples)

    file(GLOB EXAMPLE_FILES "${CMAKE_SOURCE_DIR}/examples/*.cpp")

    message(STATUS "Found examples:")

    foreach (_file ${EXAMPLE_FILES})
        # Specify target name WE=WithoutExtension
        GET_FILENAME_COMPONENT(_target "${_file}" NAME_WE)
        message(STATUS "    ${_target}...")

        add_executable(${_target} ${_file})
        target_link_libraries(${_target} em-core ${EXT_LIBRARIES})

    endforeach()

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
    file(GLOB TEST_FILES "${CMAKE_SOURCE_DIR}/test/*.cpp")

    message(STATUS "Found tests:")

    foreach (_file ${TEST_FILES})
        # Specify target name WE=WithoutExtension
        GET_FILENAME_COMPONENT(_target "${_file}" NAME_WE)
        message(STATUS "    ${_target}...")

        add_executable(${_target} ${_file})
        target_link_libraries(${_target} em-core gtest gtest_main ${EXT_LIBRARIES})
        add_test(${_target} ${_target})

    endforeach()

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

