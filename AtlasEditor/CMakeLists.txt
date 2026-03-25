set(TARGET_NAME "AtlasEditor")

# ADD FILES
file(GLOB_RECURSE ATLAS_EDITOR_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.h"
)

add_executable(${TARGET_NAME} ${ATLAS_EDITOR_SOURCES})

set(ATLAS_ICON "${CMAKE_CURRENT_SOURCE_DIR}/assets/icons/Atlas.icns")

target_sources(${TARGET_NAME} PRIVATE ${ATLAS_ICON})

set_source_files_properties(${ATLAS_ICON}
    PROPERTIES MACOSX_PACKAGE_LOCATION "Resources"
)

set_target_properties(${TARGET_NAME} PROPERTIES
    OUTPUT_NAME "Atlas Editor"
    MACOSX_BUNDLE_GUI_IDENTIFIER "com.atlas.editor"
    MACOSX_BUNDLE_BUNDLE_NAME    "Atlas Editor"
    MACOSX_BUNDLE_ICON_FILE "Atlas.icns"

    BUILD_RPATH "${CMAKE_CURRENT_BINARY_DIR}/bin;${CMAKE_BINARY_DIR}/lib"
    INSTALL_RPATH "@executable_path/../Frameworks"
    BUILD_WITH_INSTALL_RPATH TRUE
)

atlas_configure_bundle(${TARGET_NAME} "${CMAKE_CURRENT_SOURCE_DIR}/assets")
atlas_generate_script_registry(${TARGET_NAME} "${CMAKE_CURRENT_SOURCE_DIR}/src" "${CMAKE_CURRENT_SOURCE_DIR}/project")

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