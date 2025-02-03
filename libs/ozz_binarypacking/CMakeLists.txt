project(ozz_binarypacking)

set(CMAKE_CXX_STANDARD 20)

# Set to build shared libraries
set(BUILD_SHARED_LIBS FALSE)

set (SOURCES
)
add_library(${PROJECT_NAME} INTERFACE)

target_include_directories(${PROJECT_NAME}
        INTERFACE
            include
)

target_link_libraries(${PROJECT_NAME}
        INTERFACE
            glm
)

add_subdirectory(tests)
