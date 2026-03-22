cmake_minimum_required(VERSION 3.22)

set(PROJECT_NAME "Atlas")
set(VERSION_NUMBER 0.0.1)

project(${PROJECT_NAME} VERSION ${VERSION_NUMBER} LANGUAGES CXX OBJC OBJCXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)
set(CMAKE_POSITION_INDEPENDENT_CODE TRUE)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Set global output directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# GLFW build options
set(GLFW_BUILD_DOCS     OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS    OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

# Set default build type to Debug if not specified
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE Debug CACHE STRING "Build type" FORCE)
    message(STATUS "Build type not specified, defaulting to Debug")
endif()

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/Atlas/cmake")
include(AtlasBundle)
include(GenerateUserScripts)

add_subdirectory(Atlas)

if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
    add_subdirectory(AtlasEditor)
    add_subdirectory(examples/PacMan)
    add_subdirectory(examples/SandboxProject)
endif()