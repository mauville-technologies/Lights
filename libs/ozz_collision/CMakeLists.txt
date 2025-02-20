project(ozz_collision)

set(CMAKE_CXX_STANDARD 20)

# Set to build shared libraries
set(BUILD_SHARED_LIBS FALSE)

set (SOURCES
    src/shapes/ozz_circle.cpp
    src/shapes/ozz_point.cpp
    src/shapes/ozz_rectangle.cpp

    src/world.cpp
        include/ozz_collision/ozz_collision_shapes.h
)
add_library(ozz_collision STATIC
        ${SOURCES})

target_include_directories(${PROJECT_NAME}
        PUBLIC
            include
)

target_link_libraries(${PROJECT_NAME}
        PUBLIC
            glm
)

add_subdirectory(tests)
