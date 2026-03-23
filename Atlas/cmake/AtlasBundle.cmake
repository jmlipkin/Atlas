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

    add_custom_target(${TARGET}_copy_frameworks ALL
        COMMAND ${CMAKE_COMMAND} -E make_directory
            "$<TARGET_BUNDLE_CONTENT_DIR:${TARGET}>/Frameworks"
        COMMAND ${CMAKE_COMMAND} -E copy
            "$<TARGET_FILE:Atlas>"
            "$<TARGET_BUNDLE_CONTENT_DIR:${TARGET}>/Frameworks/libAtlas.dylib"
        COMMAND ${CMAKE_COMMAND} -E copy
            "$<TARGET_FILE:glad>"
            "$<TARGET_BUNDLE_CONTENT_DIR:${TARGET}>/Frameworks/libglad.dylib"
        COMMENT "Copying frameworks into ${TARGET} bundle"
    )
    add_dependencies(${TARGET} ${TARGET}_copy_frameworks)

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