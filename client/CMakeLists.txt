project(Lights_Client)

set(CMAKE_CXX_STANDARD 20)

set (SOURCES
        main.cpp
        network/client.cpp

        game/client_game.cpp

        game/scene/base_scene/base_scene.cpp
        game/scene/base_scene/objects/tilemap/tilemap.cpp
        game/scene/base_scene/objects/ground_test.cpp
        game/scene/base_scene/objects/pepe.cpp

        game/ui/debug_window.cpp
)

add_executable(${PROJECT_NAME} ${SOURCES})

# if set_base_dir(target) is defined, call it
if (COMMAND set_base_dir)
    set_base_dir(${PROJECT_NAME})
endif()

target_link_libraries(${PROJECT_NAME} Lights_Shared_Lib)
target_compile_definitions(Lights_Shared_Lib PRIVATE OZZ_RENDERER_OPENGL OZZ_WINDOW_GLFW)

target_include_directories(${PROJECT_NAME}
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
)

add_dependencies(${PROJECT_NAME} CopyAssets)
add_custom_target(CopyAssets ALL
        COMMAND ${CMAKE_COMMAND} -E echo "Copying assets..."
)

# Copy assets
set(ASSETS_DIR ${CMAKE_CURRENT_SOURCE_DIR}/assets)
add_custom_command(TARGET CopyAssets PRE_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${ASSETS_DIR}/ $<TARGET_FILE_DIR:${PROJECT_NAME}>/assets)