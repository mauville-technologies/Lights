project(Lights_Mod_Client)

set(CMAKE_CXX_STANDARD 20)

set (SOURCES main.cpp client.cpp)
add_executable(${PROJECT_NAME} ${SOURCES})

target_link_libraries(${PROJECT_NAME} Lights_Shared_Lib)