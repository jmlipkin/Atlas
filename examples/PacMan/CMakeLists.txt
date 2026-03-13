set(TARGET_NAME "PacMan")

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