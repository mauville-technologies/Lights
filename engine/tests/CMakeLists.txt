project(lights_engine_tests)

set(CMAKE_CXX_STANDARD 23)

include(FetchContent)

FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG v1.17.0
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()
include(GoogleTest)

set(SOURCES
        net/server/connection_handler_tests.cpp
)

add_executable(${PROJECT_NAME} ${SOURCES})

target_include_directories(${PROJECT_NAME}
        PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(${PROJECT_NAME}
        PRIVATE
        lights_engine
        GTest::gtest_main
)

gtest_discover_tests(${PROJECT_NAME})
