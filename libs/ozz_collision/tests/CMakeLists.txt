project(ozz_collision_tests)

set(CMAKE_CXX_STANDARD 20)

include(FetchContent)

FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

FetchContent_Declare(
        glmlib
        GIT_REPOSITORY https://github.com/g-truc/glm.git
        GIT_TAG        1.0.1
)

FetchContent_MakeAvailable(googletest glmlib)

enable_testing()

set (SOURCES
        point_collision.cpp
        circle_collision.cpp
        rectangle_collision.cpp
        polygon_collision.cpp
)
add_executable(${PROJECT_NAME} ${SOURCES})

target_link_libraries(${PROJECT_NAME} PRIVATE ozz_collision GTest::gtest_main)

include(GoogleTest)
gtest_discover_tests(${PROJECT_NAME})
