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

# add_subdirectory(extern/spdlog)
# add_subdirectory(extern/glad)
# add_subdirectory(extern/glfw)
# add_subdirectory(extern/glm)
# add_subdirectory(extern/freetype-2.14.1)

include(FetchContent)

macro(atlas_dependency NAME SUBMODULE_PATH GIT_REPO GIT_TAG)
    if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${SUBMODULE_PATH}/CMakeLists.txt")
        add_subdirectory(${SUBMODULE_PATH})
    else()
        message(STATUS "Atlas: ${NAME} submodule not found, fetching...")
        FetchContent_Declare(${NAME}
            GIT_REPOSITORY ${GIT_REPO}
            GIT_TAG        ${GIT_TAG}
            GIT_SHALLOW    TRUE
        )
        FetchContent_MakeAvailable(${NAME})
    endif()
endmacro()

atlas_dependency(spdlog    "extern/spdlog"          "https://github.com/gabime/spdlog.git"                      "v1.15.1")
atlas_dependency(glad      "extern/glad"            "https://github.com/Dav1dde/glad.git"                       "v0.1.36")
atlas_dependency(glfw      "extern/glfw"            "https://github.com/glfw/glfw.git"                          "3.4")
atlas_dependency(glm       "extern/glm"             "https://github.com/g-truc/glm.git"                         "1.0.")
atlas_dependency(freetype  "extern/freetype-2.14.1" "https://gitlab.freedesktop.org/freetype/freetype"          "2.14.1")

if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/extern/imgui/imgui.h")
    message(STATUS "Atlas: imgui submodule found")
else()
    message(STATUS "Atlas: imgui submodule not found, fetching...")
    FetchContent_Declare(imgui
        GIT_REPOSITORY "https://github.com/ocornut/imgui.git"
        GIT_TAG        "84a9d532b"
        GIT_SHALLOW    TRUE
    )
    FetchContent_Populate(imgui)
    # Copy fetched files to where Atlas expects them
    file(COPY ${imgui_SOURCE_DIR}/ DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}/extern/imgui)
endif()

if(NOT TARGET Atlas)
add_library(${TARGET_NAME} STATIC)
endif()

if(APPLE)
# Generates .metallib file
# TODO: Reconsider .metallib location
# add_custom_command(TARGET ${TARGET_NAME}
#     PRE_BUILD
#     WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/../examples/PacMan/src
#     COMMAND xcrun -sdk macosx metal -o triangle.ir -c triangle.metal
#     COMMAND xcrun -sdk macosx metal -o triangle.metallib triangle.ir
#     COMMENT "Metal shaderlibrary (triangle.metallib) built"
# )
add_custom_target(shader_library
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/assets/shaders/metal
    COMMAND xcrun -sdk macosx metal -o triangle.ir -c triangle.metal
    COMMAND xcrun -sdk macosx metal -o quad.ir -c quad.metal
    COMMAND xcrun -sdk macosx metal -o text.ir -c text.metal
    COMMAND xcrun -sdk macosx metal -o point.ir -c point.metal
    COMMAND xcrun -sdk macosx metal -o shaders.metallib triangle.ir quad.ir point.ir text.ir
    SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/assets/shaders/metal/triangle.metal
    ${CMAKE_CURRENT_SOURCE_DIR}/assets/shaders/metal/quad.metal
    ${CMAKE_CURRENT_SOURCE_DIR}/assets/shaders/metal/text.metal
)
add_dependencies(${TARGET_NAME} shader_library)
endif()

# ADD FILES
file(GLOB_RECURSE ATLAS_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.mm"
    "${CMAKE_CURRENT_SOURCE_DIR}/extern/metal-cpp/*.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/extern/metal-cpp/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/extern/metal-cpp/*.mm"
)

# Add additional specific files non-recursively
file(GLOB IMGUI_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/extern/imgui/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/extern/imgui/*.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/extern/stb_image/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/extern/stb_image/*.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/extern/entt/entt.hpp"
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
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/extern>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
    $<INSTALL_INTERFACE:include>
)

target_compile_definitions(${TARGET_NAME}
    PUBLIC

    CMAKE_BUILD_TYPE="${CMAKE_BUILD_TYPE}"
    VERSION="${CURRENT_VERSION}"

    PRODUCT_NAME_WITHOUT_VERSION="${TARGET_NAME}"

    PRIVATE
    $<$<CONFIG:Debug>:AT_DEBUG_ENABLE>
)

target_link_libraries(${TARGET_NAME}
    PUBLIC
    spdlog::spdlog
    glad
    glfw
    glm
    freetype
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