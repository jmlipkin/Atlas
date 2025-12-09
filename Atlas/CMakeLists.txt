cmake_minimum_required(VERSION 3.22)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_POSITION_INDEPENDENT_CODE TRUE)

# Enable Objective-C++ for .mm files on macOS
if(APPLE)
    enable_language(OBJC)
    enable_language(OBJCXX)
    set(CMAKE_OBJCXX_STANDARD ${CMAKE_CXX_STANDARD})
    set(CMAKE_OBJCXX_STANDARD_REQUIRED True)
endif()

set(TARGET_NAME "Atlas")

add_subdirectory(extern/spdlog)
add_subdirectory(extern/glad)
add_subdirectory(extern/glfw)
add_subdirectory(extern/glm)

add_library(${TARGET_NAME} STATIC)

# ADD FILES
file(GLOB_RECURSE ATLAS_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.mm"
    "${CMAKE_CURRENT_SOURCE_DIR}/extern/metal-cpp/*.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/extern/metal-cpp/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/extern/metal-cpp/*.mm"
)

# Add ImGui .cpp and .h (non-recursive)
file(GLOB IMGUI_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/extern/imgui/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/extern/imgui/*.h"
)

list(APPEND ATLAS_SOURCES ${IMGUI_SOURCES})

target_sources(${TARGET_NAME} PRIVATE ${ATLAS_SOURCES})

target_precompile_headers(${TARGET_NAME}
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src/atpch.h
    )

# BINARY DATA
file(GLOB_RECURSE BINARY_FILES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/assets/*")
list(FILTER BINARY_FILES EXCLUDE REGEX "/\\.DS_Store$")

target_include_directories(${TARGET_NAME} 
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/extern
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    )

target_compile_definitions(${TARGET_NAME}
    PUBLIC

    CMAKE_BUILD_TYPE="${CMAKE_BUILD_TYPE}"
    VERSION="${CURRENT_VERSION}"

    PRODUCT_NAME_WITHOUT_VERSION="${TARGET_NAME}"
)

target_link_libraries(${TARGET_NAME}
    PUBLIC
    spdlog::spdlog
    glad
    glfw
    glm
)

if(APPLE)
    target_link_libraries(${TARGET_NAME}
        PUBLIC
            "-framework AppKit"
            "-framework Foundation"
            "-framework QuartzCore"
            "-framework Metal"
    )
endif()