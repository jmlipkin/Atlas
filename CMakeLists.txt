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

# -----------------------------------------------------------------------------
# atlas_configure_bundle(<target> [assets_dir])
#
# Marks <target> as a macOS app bundle and copies Atlas metal shader library
# into the Resources directory.
# If an optional <assets_dir> is provided, that directory is also copied into
# Resources/assets.
#
# Requires ATLAS_METALLIB to be set (exported by Atlas/CMakeLists.txt).
#
# -----------------------------------------------------------------------------
function(atlas_configure_bundle TARGET)
    if(NOT APPLE)
        return()
    endif()

    set_target_properties(${TARGET} PROPERTIES MACOSX_BUNDLE TRUE)

    # Copy Metal shader library
    add_custom_command(TARGET ${TARGET} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E make_directory
            "$<TARGET_BUNDLE_CONTENT_DIR:${TARGET}>/Resources"
        COMMAND ${CMAKE_COMMAND} -E copy
            "${ATLAS_METALLIB}"
            "$<TARGET_BUNDLE_CONTENT_DIR:${TARGET}>/Resources/default.metallib"
        COMMENT "Copying Atlas shader library into ${TARGET} bundle"
    )

    # Copy Atlas engine resources (fonts, etc)
    add_custom_command(TARGET ${TARGET} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${ATLAS_RESOURCES_DIR}"
            "$<TARGET_BUNDLE_CONTENT_DIR:${TARGET}>/Resources/Atlas"
        COMMENT "Copying Atlas engine resources into ${TARGET} bundle"
    )

    # Optional: copy caller-provided assets directory
    if(ARGC GREATER 1)
        add_custom_command(TARGET ${TARGET} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_directory
                "${ARGV1}"
                "$<TARGET_BUNDLE_CONTENT_DIR:${TARGET}>/Resources/assets"
            COMMENT "Copying game assets into ${TARGET} bundle"
        )
    endif()
endfunction()

add_subdirectory(Atlas)

if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
    add_subdirectory(AtlasEditor)
    add_subdirectory(examples/PacMan)
    add_subdirectory(examples/SandboxProject)
endif()