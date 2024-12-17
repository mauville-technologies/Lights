cmake_minimum_required(VERSION 3.29)
project(Lights)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/dist/${CMAKE_SYSTEM_NAME}/${CMAKE_BUILD_TYPE})

add_subdirectory(shared)
add_subdirectory(server)
add_subdirectory(mod_client)

add_subdirectory(cli_tool)
