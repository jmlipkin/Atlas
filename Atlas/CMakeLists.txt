set(TARGET_NAME "Atlas")

set(JSON_BuildTests OFF CACHE INTERNAL "")

set(ATLAS_ROOT_DIR "${CMAKE_SOURCE_DIR}")
set(ATLAS_CMAKE_COMMAND "${CMAKE_COMMAND}")

include(FetchContent)

# -----------------------------------------------------------------------------
#
# atlas_dependency(<name> <submodule_path> <git_repo> <git_tag>)
#
# Uses a vendored submodule if present, otherwise fetches via FetchContent.
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

atlas_dependency(json       "extern/json"            "https://github.com/nlohmann/json.git"                      "v3.12.0")
atlas_dependency(spdlog     "extern/spdlog"          "https://github.com/gabime/spdlog.git"                      "v1.15.1")
atlas_dependency(glad       "extern/glad"            "https://github.com/Dav1dde/glad.git"                       "v0.1.36")
atlas_dependency(glfw       "extern/glfw"            "https://github.com/glfw/glfw.git"                          "3.4")
atlas_dependency(glm        "extern/glm"             "https://github.com/g-truc/glm.git"                         "1.0.")
atlas_dependency(freetype   "extern/freetype-2.14.1" "https://gitlab.freedesktop.org/freetype/freetype"          "2.14.1")
atlas_dependency(libsndfile "extern/libsndfile"      "https://github.com/libsndfile/libsndfile.git"              "1.2.2")

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
    file(COPY ${imgui_SOURCE_DIR}/ DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}/extern/imgui)
endif()

if(NOT TARGET Atlas)
    add_library(${TARGET_NAME} SHARED)
endif()

set_target_properties(${TARGET_NAME} PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)

# On macOS embed the install name as @rpath so bundles can override it.
if(APPLE)
    set_target_properties(${TARGET_NAME} PROPERTIES
        INSTALL_NAME_DIR "@rpath"
        BUILD_WITH_INSTALL_NAME_DIR TRUE
    )
endif()

# -----------------------------------------------------------------------------
#
# Metal shader library
#
# Outputs default.metallib into the binary directory, then exposed via
# ATLAS_METALLIB for atlas_configure_bundle() to copy into each bundle.
#
# -----------------------------------------------------------------------------
if(APPLE)
    set(ATLAS_METAL_SRC    "${CMAKE_CURRENT_SOURCE_DIR}/assets/shaders/metal")
    set(ATLAS_METALLIB_OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/default.metallib")

    add_custom_target(shader_library
        COMMAND xcrun -sdk macosx metal
            -o ${CMAKE_CURRENT_BINARY_DIR}/polygon.ir
            -c ${ATLAS_METAL_SRC}/polygon.metal
        COMMAND xcrun -sdk macosx metal
            -o ${CMAKE_CURRENT_BINARY_DIR}/text.ir
            -c ${ATLAS_METAL_SRC}/text.metal

        COMMAND xcrun -sdk macosx metallib
            -o "${ATLAS_METALLIB_OUTPUT}"
            "${CMAKE_CURRENT_BINARY_DIR}/polygon.ir"
            "${CMAKE_CURRENT_BINARY_DIR}/text.ir"
        SOURCES
            "${ATLAS_METAL_SRC}/polygon.metal"
            "${ATLAS_METAL_SRC}/text.metal"
    )
    add_dependencies(${TARGET_NAME} shader_library)

    set(ATLAS_METALLIB "${ATLAS_METALLIB_OUTPUT}" PARENT_SCOPE)
endif()

set(ATLAS_RESOURCES_DIR "${CMAKE_CURRENT_SOURCE_DIR}/assets" PARENT_SCOPE)

# -----------------------------------------------------------------------------
# Source files
# -----------------------------------------------------------------------------
file(GLOB_RECURSE ATLAS_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.mm"
    "${CMAKE_CURRENT_SOURCE_DIR}/extern/metal-cpp/*.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/extern/metal-cpp/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/extern/metal-cpp/*.mm"
)

file(GLOB IMGUI_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/extern/imgui/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/extern/imgui/*.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/extern/imgui/misc/cpp/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/extern/imgui/misc/cpp/*.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/extern/stb_image/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/extern/stb_image/*.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/extern/entt/entt.hpp"
)

list(APPEND ATLAS_SOURCES ${IMGUI_SOURCES})
target_sources(${TARGET_NAME} PRIVATE ${ATLAS_SOURCES})

# -----------------------------------------------------------------------------
# Per-file compile options
# -----------------------------------------------------------------------------
set_source_files_properties(
    "${CMAKE_CURRENT_SOURCE_DIR}/src/Atlas/Platform/Metal/ImGuiMetalRenderer.mm"
    PROPERTIES COMPILE_OPTIONS "-Wno-arc-bridge-casts-disallowed-in-nonarc"
)

set_source_files_properties(
    "${CMAKE_CURRENT_SOURCE_DIR}/extern/imgui/misc/cpp/imgui_stdlib.cpp"
    PROPERTIES INCLUDE_DIRECTORIES "${CMAKE_CURRENT_SOURCE_DIR}/extern/imgui"
)

# -----------------------------------------------------------------------------
# Precompiled header
# -----------------------------------------------------------------------------
target_precompile_headers(${TARGET_NAME}
    PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/src/atpch.h"
)

# -----------------------------------------------------------------------------
# Include directories
# -----------------------------------------------------------------------------
target_include_directories(${TARGET_NAME}
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/extern>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/generated>
    PRIVATE
        # imgui stdlib extension needs imgui root on its include path
        "${CMAKE_CURRENT_SOURCE_DIR}/extern/imgui"
)

# -----------------------------------------------------------------------------
# Compile definitions
#
# ATLAS_BUILD_DLL  — marks Atlas symbols as dllexport on Windows, and enables
#                    the ATLAS_API visibility attribute on macOS/Linux.
# AT_DEBUG_ENABLE  — debug-only diagnostics.
# -----------------------------------------------------------------------------
target_compile_definitions(${TARGET_NAME}
    PUBLIC
        CMAKE_BUILD_TYPE="${CMAKE_BUILD_TYPE}"
        VERSION="${CURRENT_VERSION}"
        PRODUCT_NAME_WITHOUT_VERSION="${TARGET_NAME}"
    PRIVATE
        ATLAS_BUILD_DLL
        $<$<CONFIG:Debug>:AT_DEBUG_ENABLE>
)

# TODO: Re-enable with selective ATLAS_API symbol export once all
# -----------------------------------------------------------------------------
# Visibility — hide all symbols by default; only ATLAS_API ones are exported.
# This keeps the dylib surface clean and avoids collisions with script dylibs.
# -----------------------------------------------------------------------------
# if(APPLE OR UNIX)
    # target_compile_options(${TARGET_NAME} PRIVATE -fvisibility=hidden)
    # target_compile_options(${TARGET_NAME} PRIVATE -fvisibility-inlines-hidden)
# endif()

# -----------------------------------------------------------------------------
# Linker options
#
# -undefined dynamic_lookup (macOS) / --allow-shlib-undefined (Linux)
#
# Atlas calls createApplication() which is defined by the client executable,
# not by Atlas itself. These flags tell the linker that it is acceptable for
# this symbol to be resolved at runtime by the host process.
# -----------------------------------------------------------------------------
if(APPLE)
    target_link_options(${TARGET_NAME} PRIVATE -undefined dynamic_lookup)
elseif(UNIX)
    target_link_options(${TARGET_NAME} PRIVATE -Wl,--allow-shlib-undefined)
endif()

# -----------------------------------------------------------------------------
# Generated headers (Version.h)
# -----------------------------------------------------------------------------
configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/src/Atlas/Core/Version.h.in"
    "${CMAKE_CURRENT_BINARY_DIR}/generated/Version.h"
)

configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/src/Atlas/Core/AtlasPaths.h.in"
    "${CMAKE_CURRENT_BINARY_DIR}/generated/AtlasPaths.h"
)
# -----------------------------------------------------------------------------
# Linked libraries
#
# All dependencies are PUBLIC so that consumer targets (AtlasEditor, PacMan,
# etc.) automatically inherit their include paths and link flags — they do not
# need to repeat these in their own target_link_libraries calls.
# -----------------------------------------------------------------------------
target_link_libraries(${TARGET_NAME}
    PUBLIC
        spdlog::spdlog
        glad
        glfw
        glm
        freetype
        nlohmann_json::nlohmann_json
        libsndfile
)

if(APPLE)
    target_link_libraries(${TARGET_NAME}
        PUBLIC
            "-framework AppKit"
            "-framework Foundation"
            "-framework QuartzCore"
            "-framework Metal"
            "-framework UniformTypeIdentifiers"
            "-framework CoreAudio"
            "-framework AudioToolbox"
    )
endif()