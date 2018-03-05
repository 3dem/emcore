cmake_minimum_required(VERSION 3.0)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
add_compile_options(-std=c++11)

message(STATUS "Setting CXX supported standard '${CMAKE_CXX_STANDARD}'")

project(em-core VERSION 0.0.1)

# Add option to build examples, turn on with 'cmake -DBUILD_TESTS=ON'
option(BUILD_EXAMPLES "Build all examples." OFF)
# Add option to build tests, turn on with 'cmake -DBUILD_TESTS=ON'
option(BUILD_TESTS "Build all tests." OFF)
# Add option to build the doxygen documentation, turn on with 'cmake -DBUILD_DOCS=ON'
option(BUILD_DOCS "Build the doxygen documentation" OFF)
# Add option to decide if build tiff or not, useful when the library is not detected
option(BUILD_TIFF "Build library with TIFF support" ON)
# Build docopt or not
option(BUILD_DOCOPT "Build library with docopt support" OFF)
# Build static library
option(BUILD_STATIC "Build static library." OFF)

string(TIMESTAMP EM_CORE_TIMESTAMP "%d-%m-%y %H:%M:%S")

add_definitions(-DEM_CORE_VERSION="em-core v${PROJECT_VERSION}") # To be used in c++ code
add_definitions(-DEM_CORE_TIMESTAMP="build ${EM_CORE_TIMESTAMP}")

set(PROJECT_PATH  "${CMAKE_CURRENT_SOURCE_DIR}")
set(INCLUDE_PATH  "${PROJECT_PATH}/include")
set(SRC_PATH      "${PROJECT_PATH}/src")
set(BUILD_PATH    "${PROJECT_PATH}/build")
set(TEST_SRC_PATH "${PROJECT_PATH}/tests")
set(EXAMPLE_PATH "${PROJECT_PATH}/examples")
set(DOC_PATH      "${PROJECT_PATH}/docs")
set(EXT_PATH      "${PROJECT_PATH}/libs")

include_directories(AFTER "${INCLUDE_PATH}")

#file(GLOB HEADER_FILES "${INCLUDE_PATH}/*/*.h") # This is not necessary

# File lists definitions
file(GLOB SOURCE_FILES "${SRC_PATH}/*/*.cpp")
file(GLOB EXAMPLE_FILES "${EXAMPLE_PATH}/*.cpp")

# Adding external project functions
include(ExternalProject)

set(EXT_LIBRARIES ) # Defined empty to be fullfiled, if necessary

###################################
# External libraries to be included
###################################
if (BUILD_DOCOPT)
    set(DOCOPT_PATH "${EXT_PATH}/docopt")

        if ( NOT EXISTS ${DOCOPT_PATH}/docopt.cpp )
            MESSAGE( STATUS " Update submodule" )
            execute_process(COMMAND git submodule update --init ${DOCOPT_PATH} )
        endif()

    include_directories(AFTER "${DOCOPT_PATH}")
    list(APPEND SOURCE_FILES "${DOCOPT_PATH}/docopt.cpp")
else ()
    list(REMOVE_ITEM EXAMPLE_FILES "${EXAMPLE_PATH}/em-image.cpp")
endif()

################################
# External system libraries
################################
find_package(TIFF)
if (TIFF_FOUND AND BUILD_TIFF)

    include_directories(${TIFF_INCLUDE_DIRS})
    set(EXT_LIBRARIES ${EXT_LIBRARIES} ${TIFF_LIBRARIES})

    MESSAGE( STATUS "TIFF library: " ${TIFF_LIBRARIES} )
    MESSAGE( STATUS "TIFF include: " ${TIFF_INCLUDE_DIRS} )

else ()
    list(REMOVE_ITEM SOURCE_FILES "${SRC_PATH}/image/image_tiff.cpp")
endif ()

#find_package(JPEG)
#find_package(PNG)

if (BUILD_STATIC)
    add_library(em-core STATIC ${SOURCE_FILES} ${HEADER_FILES})
else()
    add_library(em-core SHARED ${SOURCE_FILES} ${HEADER_FILES})
endif()

target_link_libraries( em-core ${EXT_LIBRARIES} )


################################
# Examples
################################
if (BUILD_EXAMPLES)
    #add_subdirectory(examples)

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
    set(GOOGLETEST_PATH "${EXT_PATH}/googletest")

    if ( NOT EXISTS ${GOOGLETEST_PATH}/googletest )
        MESSAGE( STATUS " Update submodule" )
        execute_process(COMMAND git submodule update --init ${GOOGLETEST_PATH} )
    endif()

    # This adds another subdirectory, which has 'project(gtest)'.
    add_subdirectory(libs/googletest/googletest)

    enable_testing()

    # Include the gtest library. gtest_SOURCE_DIR is available due to
    # 'project(gtest)' above.
    include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})

    ##############
    # Unit Tests
    ##############
    file(GLOB TEST_FILES "${CMAKE_SOURCE_DIR}/tests/*.cpp")

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

    if (NOT DOXYGEN_FOUND)
        message(FATAL_ERROR
                "Doxygen is needed to build the documentation.
                Please make sure it is installed.")
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

