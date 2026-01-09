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

# Generates .metallib file
# TODO: Reconsider .metallib location
# add_custom_command(TARGET ${TARGET_NAME}
#     PRE_BUILD
#     WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/../examples/PacMan/src
#     COMMAND xcrun -sdk macosx metal -o triangle.ir -c triangle.metal
#     COMMAND xcrun -sdk macosx metal -o triangle.metallib triangle.ir
#     COMMENT "Metal shaderlibrary (triangle.metallib) built"
# )
add_custom_target(triangle_shader
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/../examples/PacMan/src
    COMMAND xcrun -sdk macosx metal -o triangle.ir -c triangle.metal
    COMMAND xcrun -sdk macosx metal -o quad.ir -c quad.metal
    COMMAND xcrun -sdk macosx metal -o shaders.metallib triangle.ir quad.ir
    SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/../examples/PacMan/src/triangle.metal
    ${CMAKE_CURRENT_SOURCE_DIR}/../examples/PacMan/src/quad.metal
)
add_dependencies(${TARGET_NAME} triangle_shader)

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

# Silence ImGui + Metal-cpp bridging warning
set_source_files_properties(
    src/Atlas/Platform/Metal/ImGuiMetalRenderer.mm
    PROPERTIES
    COMPILE_OPTIONS "-Wno-arc-bridge-casts-disallowed-in-nonarc"
)

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

    PRIVATE
    $<$<CONFIG:Debug>:AT_DEBUG>
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