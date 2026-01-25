cmake_minimum_required(VERSION 3.22)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(PROJECT_NAME "Atlas")
set(TARGET_NAME "PacMan")
set(VERSION_NUMBER 0.0.1)

project(${PROJECT_NAME} VERSION ${VERSION_NUMBER} LANGUAGES CXX OBJC OBJCXX)

# Set global output directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

add_subdirectory(Atlas)
add_subdirectory(AtlasEditor)
add_subdirectory(examples/Sandbox)
add_subdirectory(examples/PacMan)