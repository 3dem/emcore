cmake_minimum_required(VERSION 3.0)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
add_compile_options(-std=c++11)

message(STATUS "Setting CXX supported standard '${CMAKE_CXX_STANDARD}'")

# Read the version from a single point (VERSION.txt file)
file (STRINGS "${CMAKE_CURRENT_SOURCE_DIR}/../VERSION.txt" EMCORE_VERSION)

project(em-core VERSION ${VERSION})


add_definitions(-DEMCORE_VERSION="${EMCORE_VERSION}") # To be used in c++ code
add_definitions(-DEMCORE_VERSION_STRING="em-core v${EMCORE_VERSION}") # To be used in c++ code
add_definitions(-DVERSION_INFO="${PROJECT_VERSION}") # To be used in c++ code
add_definitions(-DEM_CORE_TIMESTAMP="build ${EM_CORE_TIMESTAMP}")


set(PROJECT_PATH  "${CMAKE_CURRENT_SOURCE_DIR}/..")
set(INCLUDE_PATH  "${PROJECT_PATH}/include")
set(BUILD_PATH    "${PROJECT_BINARY_DIR}")
set(EXT_PATH      "${PROJECT_PATH}/libs")
# Test the following
SET(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib")
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

MESSAGE( STATUS "Module path: " ${CMAKE_MODULE_PATH} )

include_directories(AFTER "${INCLUDE_PATH}")

set(PYBIND_PATH        "${PROJECT_PATH}/python")
set(PYBIND_SRC_PATH    "${PYBIND_PATH}/src")
set(PYBIND_TESTS_PATH  "${PYBIND_PATH}/tests")
set(PYBIND11_PATH  "${EXT_PATH}/pybind11")

if ( NOT EXISTS ${PYBIND11_PATH}/pybind11 )
    MESSAGE( STATUS " Update submodule ${PYBIND11_PATH}" )
    execute_process(COMMAND git submodule update --init ${PYBIND11_PATH} )
endif()

add_subdirectory(${PYBIND11_PATH} pybind11)

file(GLOB PYBIND_SOURCE_FILES "${PYBIND_SRC_PATH}/*.cpp")

pybind11_add_module(emcoreBinding "${PYBIND_SOURCE_FILES}")

find_library(EMCORE emcore ${BUILD_PATH})
if (EMCORE-NOTFOUND)
    MESSAGE( STATUS "emcore library NOT FOUND." )
else ()
    MESSAGE( STATUS "emcore found: ${EMCORE}" )
    target_link_libraries(emcoreBinding PRIVATE ${EMCORE})
endif ()


if (BUILD_STATIC)
    set_property(TARGET emcoreBinding PROPERTY OUTPUT_NAME "_emcore.a")
else()
    set_property(TARGET emcoreBinding PROPERTY OUTPUT_NAME "_emcore.so")
endif()

set_property(TARGET emcoreBinding PROPERTY PREFIX "")
set_property(TARGET emcoreBinding PROPERTY SUFFIX "")

install(TARGETS emcoreBinding LIBRARY DESTINATION emcore)

# Install programs as part of the module
file(GLOB EMCORE_PROGRAMS "${PROJECT_PATH}/build/em-*")

#foreach (_file ${EMCORE_PROGRAMS})
#     message(STATUS "    ${_file}")
#endforeach()

install(FILES ${EMCORE_PROGRAMS} DESTINATION bin)

## Find all test under /tests folder and add them as test-cases
#enable_testing()
#file(GLOB PYBIND_TEST_FILES "${PYBIND_TESTS_PATH}/pytest_*.py")
#message(STATUS "Found tests:")
#
#foreach (_file ${PYBIND_TEST_FILES})
#    # Specify target name WE=WithoutExtension
#    GET_FILENAME_COMPONENT(_target "${_file}" NAME_WE)
#    message(STATUS "    ${_target}...")
#    add_test(NAME ${_target} COMMAND ${PYTHON_EXECUTABLE} ${_file})
#endforeach()

