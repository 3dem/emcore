cmake_minimum_required(VERSION 3.0)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
add_compile_options(-std=c++11)

message(STATUS "Setting CXX supported standard '${CMAKE_CXX_STANDARD}'")

project(em-core VERSION 0.0.3)

# Add option to build examples, turn on with 'cmake -DBUILD_TESTS=ON'
option(BUILD_EXAMPLES "Build all examples." OFF)
# Add option to build tests, turn on with 'cmake -DBUILD_TESTS=ON'
option(BUILD_TESTS "Build all tests." OFF)

option(BUILD_DOCOPT "Build library with docopt support" ON)
# Add option to build the doxygen documentation, turn on with 'cmake -DBUILD_DOCS=ON'
option(BUILD_DOCS "Build the doxygen documentation" OFF)
# Add option to decide if build tiff or not
option(BUILD_TIFF "Build library with TIFF support" ON)
# Add option to decide if build png or not
option(BUILD_PNG "Build library with PNG support" ON)
# Add option to decide if build jpeg or not
option(BUILD_JPEG "Build library with JPEG support" ON)
# Build Python binding or not
option(BUILD_PYBIND "Build library with Python binding support" OFF)
# Build static library
option(BUILD_STATIC "Build static library." OFF)

string(TIMESTAMP EM_CORE_TIMESTAMP "%Y-%m-%d %H:%M:%S")

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

# Test the following
SET(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib")

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

MESSAGE( STATUS "Module path: " ${CMAKE_MODULE_PATH} )

include_directories(AFTER "${INCLUDE_PATH}")

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

find_package(JPEG)
if (JPEG_FOUND AND BUILD_JPEG)
    include_directories(${JPEG_INCLUDE_DIR})
    set(EXT_LIBRARIES ${EXT_LIBRARIES} ${JPEG_LIBRARIES})

    MESSAGE( STATUS "JPEG library: " ${JPEG_LIBRARIES} )
    MESSAGE( STATUS "JPEG include: " ${JPEG_INCLUDE_DIR} )

else ()
    MESSAGE( STATUS "JPEG NOT FOUND." )
    list(REMOVE_ITEM SOURCE_FILES "${SRC_PATH}/image/image_jpeg.cpp")
endif ()

find_package(PNG)
if (PNG_FOUND AND BUILD_PNG)

    include_directories(${PNG_INCLUDE_DIRS})
    set(EXT_LIBRARIES ${EXT_LIBRARIES} ${PNG_LIBRARIES})

    MESSAGE( STATUS "PNG library: " ${PNG_LIBRARIES} )
    MESSAGE( STATUS "PNG include: " ${PNG_INCLUDE_DIRS} )

else ()
    MESSAGE( STATUS "PNG NOT FOUND." )
    list(REMOVE_ITEM SOURCE_FILES "${SRC_PATH}/image/image_png.cpp")
endif ()

if (BUILD_STATIC)
    add_library(em-core STATIC ${SOURCE_FILES} ${HEADER_FILES})
    set_property(TARGET em-core PROPERTY OUTPUT_NAME "emcore.a")
else()
    add_library(em-core SHARED ${SOURCE_FILES} ${HEADER_FILES})
    set_property(TARGET em-core PROPERTY OUTPUT_NAME "emcore.so")
endif()

set_property(TARGET em-core PROPERTY PREFIX "lib")
set_property(TARGET em-core PROPERTY SUFFIX "")


#############################
#  FFTW3
#############################
find_package(FFTW)

if (FFTW_FOUND)
    message(STATUS "FFTW3 library: " ${FFTW_LIB} ${FFTWF_LIB})
    message(STATUS "FFTW3 include: " ${FFTW_INCLUDES})
    include_directories(${FFTW_INCLUDES})
    set(EXT_LIBRARIES ${EXT_LIBRARIES} ${FFTW_LIB} ${FFTWF_LIB})
endif(FFTW_FOUND)

#############################
#  SQlite3
#############################
find_package (sqlite3)
if (SQLITE3_FOUND)
    MESSAGE( STATUS "SQLite3 library: " ${SQLITE3_LIBRARIES} )
    MESSAGE( STATUS "SQLite3 include: " ${SQLITE3_INCLUDE_DIRS} )
    include_directories(${SQLITE3_INCLUDE_DIRS})
    set(EXT_LIBRARIES ${EXT_LIBRARIES} ${SQLITE3_LIBRARIES})
endif (SQLITE3_FOUND)

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
        install(TARGETS ${_target} DESTINATION bin)
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

################################
# Python bindings
################################
if(BUILD_PYBIND)

    set(PYBIND_PATH        "${PROJECT_PATH}/python")
    set(PYBIND_SRC_PATH    "${PYBIND_PATH}/src")
    set(PYBIND_TESTS_PATH  "${PYBIND_PATH}/tests")
    set(PYBIND11_PATH  "${EXT_PATH}/pybind11")


    if ( NOT EXISTS ${PYBIND11_PATH}/pybind11 )
        MESSAGE( STATUS " Update submodule ${PYBIND11_PATH}" )
        execute_process(COMMAND git submodule update --init ${PYBIND11_PATH} )
    endif()

    add_subdirectory(${PYBIND11_PATH})

    file(GLOB PYBIND_SOURCE_FILES "${PYBIND_SRC_PATH}/*.cpp")

    pybind11_add_module(em "${PYBIND_SOURCE_FILES}")

    target_link_libraries(em PRIVATE em-core)

    if (BUILD_STATIC)
        set_property(TARGET em PROPERTY OUTPUT_NAME "emcore.a")
    else()
        set_property(TARGET em PROPERTY OUTPUT_NAME "emcore.so")
    endif()

    set_property(TARGET em PROPERTY PREFIX "")
    set_property(TARGET em PROPERTY SUFFIX "")

    install(TARGETS em DESTINATION lib)

    # Find all test under /tests folder and add them as test-cases
    enable_testing()
    file(GLOB PYBIND_TEST_FILES "${PYBIND_TESTS_PATH}/pytest_*.py")
    message(STATUS "Found tests:")

    foreach (_file ${PYBIND_TEST_FILES})
        # Specify target name WE=WithoutExtension
        GET_FILENAME_COMPONENT(_target "${_file}" NAME_WE)
        message(STATUS "    ${_target}...")
        add_test(NAME ${_target} COMMAND ${PYTHON_EXECUTABLE} ${_file})
    endforeach()

endif(BUILD_PYBIND)

install(TARGETS em-core DESTINATION lib)
install(DIRECTORY ${INCLUDE_PATH}/emc DESTINATION include)
