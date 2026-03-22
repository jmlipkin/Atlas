set(TARGET_NAME "SandboxApp")

# ADD FILES
file(GLOB_RECURSE PACMAN_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.h"
)

add_executable(${TARGET_NAME} ${PACMAN_SOURCES})

set_target_properties(${TARGET_NAME} PROPERTIES
    MACOSX_BUNDLE_GUI_IDENTIFIER "com.atlas.pacman"
    MACOSX_BUNDLE_BUNDLE_NAME    "PacMan"
)

# Configure as macOS app bundle, copy Atlas resources and game assets
atlas_configure_bundle(${TARGET_NAME} "${CMAKE_CURRENT_SOURCE_DIR}/assets")

atlas_generate_script_registry(${TARGET_NAME} "${CMAKE_CURRENT_SOURCE_DIR}/src" "${CMAKE_CURRENT_SOURCE_DIR}/project")

# Copy project files into bundle
if(APPLE)
    add_custom_command(TARGET ${TARGET_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${CMAKE_CURRENT_SOURCE_DIR}/project"
            "$<TARGET_BUNDLE_CONTENT_DIR:${TARGET_NAME}>/Resources/project"
        COMMENT "Copying PacMan project files into bundle"
    )
endif()

target_include_directories(${TARGET_NAME} 
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/extern
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_compile_definitions(${TARGET_NAME}
    PRIVATE
        CMAKE_BUILD_TYPE="${CMAKE_BUILD_TYPE}"
        VERSION="${CURRENT_VERSION}"
)

target_link_libraries(${TARGET_NAME}
    PRIVATE
        Atlas
)