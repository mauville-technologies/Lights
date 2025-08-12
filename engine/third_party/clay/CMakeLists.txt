project(clay)

# set to latest C standard
set(CMAKE_C_STANDARD 23)
add_library(${PROJECT_NAME} clay.cpp)

target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})