project(ozz_binarypacking_tests)

set(CMAKE_CXX_STANDARD 20)

# import google test
include(FetchContent)
FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)

set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()

set (SOURCES
        empty.cpp
)
add_executable(${PROJECT_NAME} ${SOURCES})

target_link_libraries(${PROJECT_NAME} PRIVATE ozz_binarypacking GTest::gtest_main)

include(GoogleTest)
gtest_discover_tests(${PROJECT_NAME})
