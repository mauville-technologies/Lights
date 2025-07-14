project(Lights_CLI)

set(CMAKE_CXX_STANDARD 20)

add_executable(${PROJECT_NAME} main.cpp)

# if set_base_dir(target) is defined, call it
if (COMMAND set_base_dir)
    set_base_dir(${PROJECT_NAME} ${CMAKE_SOURCE_DIR})
endif()

target_link_libraries(${PROJECT_NAME} lights_engine)
