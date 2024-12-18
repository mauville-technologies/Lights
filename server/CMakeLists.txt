project(Lights_Server)

set(CMAKE_CXX_STANDARD 20)

set (SOURCES
        main.cpp
        network/server.cpp
        network/connected_client.cpp

        game/game.cpp
        game/player.cpp

        database/database.cpp
)
add_executable(${PROJECT_NAME} ${SOURCES})

target_link_libraries(${PROJECT_NAME} Lights_Shared_Lib)

target_include_directories(${PROJECT_NAME}
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
)