project(Lights_Client)

set(CMAKE_CXX_STANDARD 20)


set (SOURCES
        main.cpp
        network/client.cpp

        game/client_game.cpp
        game/rendering/renderer.cpp

        game/scene/main_menu/main_menu_scene.cpp
)
add_executable(${PROJECT_NAME} ${SOURCES})

# if set_base_dir(target) is defined, call it
if (COMMAND set_base_dir)
    set_base_dir(${PROJECT_NAME})
endif()

target_link_libraries(${PROJECT_NAME} Lights_Shared_Lib)

target_include_directories(${PROJECT_NAME}
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
)
