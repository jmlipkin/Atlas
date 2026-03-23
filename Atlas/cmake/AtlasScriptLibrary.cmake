function(atlas_build_script_library TARGET_NAME SRC_DIR)
    file(GLOB_RECURSE SCRIPT_SOURCES
        "${SRC_DIR}/*.cpp"
        "${SRC_DIR}/*.h"
        "${SRC_DIR}/*.mm"
    )

    # Exclude app entry points
    list(FILTER SCRIPT_SOURCES EXCLUDE REGEX ".*App\\.cpp$")
    list(FILTER SCRIPT_SOURCES EXCLUDE REGEX ".*main\\.cpp$")

    set(GENERATED_CPP "${CMAKE_CURRENT_BINARY_DIR}/generated/UserScripts.cpp")

    add_library(${TARGET_NAME}Scripts SHARED ${SCRIPT_SOURCES} ${GENERATED_CPP})

    target_link_libraries(${TARGET_NAME}Scripts PRIVATE Atlas)

    target_include_directories(${TARGET_NAME}Scripts PRIVATE "${SRC_DIR}" "${CMAKE_CURRENT_BINARY_DIR}/generated")

    if(APPLE)
        set_target_properties(${TARGET_NAME}Scripts PROPERTIES
            LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
            BUILD_RPATH "${CMAKE_BINARY_DIR}/bin"
            INSTALL_RPATH "@loader_path/../Frameworks"
            BUILD_WITH_INSTALL_RPATH TRUE
        )
    endif()

    # Copy to project directory after build
    add_custom_command(TARGET ${TARGET_NAME}Scripts POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
            "$<TARGET_FILE:${TARGET_NAME}Scripts>"
            "${CMAKE_CURRENT_SOURCE_DIR}/Project/GameScripts.dylib"
        COMMENT "Copying ${TARGET_NAME} script library to project directory"
    )
endfunction()