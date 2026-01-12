cmake_minimum_required(VERSION 3.22)

set(LIBRARY_NAME glad)

project(${LIBRARY_NAME} VERSION 0.0.1)

include(GNUInstallDirs)

# ADD FILES
file(GLOB_RECURSE SOURCE_FILES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.h"
)

add_library(${LIBRARY_NAME} SHARED ${SOURCE_FILES})

target_sources(${LIBRARY_NAME} PRIVATE ${SOURCE_FILES})

target_include_directories(${LIBRARY_NAME}
    PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include)