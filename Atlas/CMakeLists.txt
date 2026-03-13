set(TARGET_NAME "Atlas")

set(JSON_BuildTests OFF CACHE INTERNAL "")

include(FetchContent)

# -----------------------------------------------------------------------------
#
# atlas_dependency(<name> <submodule_path> <git_repo> <git_tag>)
#
# Uses a vendored submodule if present, otherwises fetches via FetchContent.
#
# -----------------------------------------------------------------------------
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

atlas_dependency(json "extern/json" "https://github.com/nlohmann/json.git" "v3.12.0")
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

# -----------------------------------------------------------------------------
#
# Metal shader library
#
# Outputs default.metallib into the build directory
#
# -----------------------------------------------------------------------------
if(APPLE)
    set(ATLAS_METAL_SRC "${CMAKE_CURRENT_SOURCE_DIR}/assets/shaders/metal")
    set(ATLAS_METALLIB_OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/default.metallib")

    add_custom_target(shader_library
        COMMAND xcrun -sdk macosx metal
            -o ${CMAKE_CURRENT_BINARY_DIR}/triangle.ir
            -c ${ATLAS_METAL_SRC}/triangle.metal
        COMMAND xcrun -sdk macosx metal
            -o ${CMAKE_CURRENT_BINARY_DIR}/quad.ir
            -c ${ATLAS_METAL_SRC}/quad.metal
        COMMAND xcrun -sdk macosx metal
            -o ${CMAKE_CURRENT_BINARY_DIR}/text.ir
            -c ${ATLAS_METAL_SRC}/text.metal
        COMMAND xcrun -sdk macosx metal
            -o ${CMAKE_CURRENT_BINARY_DIR}/point.ir
            -c ${ATLAS_METAL_SRC}/point.metal

        COMMAND xcrun -sdk macosx metallib
            -o "${ATLAS_METALLIB_OUTPUT}"
            "${CMAKE_CURRENT_BINARY_DIR}/triangle.ir"
            "${CMAKE_CURRENT_BINARY_DIR}/quad.ir"
            "${CMAKE_CURRENT_BINARY_DIR}/text.ir"
            "${CMAKE_CURRENT_BINARY_DIR}/point.ir"
        
        SOURCES
        ${ATLAS_METAL_SRC}/triangle.metal
        ${ATLAS_METAL_SRC}/quad.metal
        ${ATLAS_METAL_SRC}/text.metal
        ${ATLAS_METAL_SRC}/point.metal
    )
    add_dependencies(${TARGET_NAME} shader_library)

    set(ATLAS_METALLIB "${ATLAS_METALLIB_OUTPUT}" PARENT_SCOPE)
endif()

set(ATLAS_RESOURCES_DIR "${CMAKE_CURRENT_SOURCE_DIR}/assets" PARENT_SCOPE)

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
    
    $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/generated>
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
    nlohmann_json::nlohmann_json
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