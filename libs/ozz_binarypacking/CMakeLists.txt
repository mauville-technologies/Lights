project(ozz_binarypacking)

set(CMAKE_CXX_STANDARD 23)

# Set to build shared libraries

set(SOURCES
)

file(GLOB_RECURSE HEADERS
        "include/*.h"
)

add_library(${PROJECT_NAME} INTERFACE ${HEADERS})

target_include_directories(${PROJECT_NAME}
        INTERFACE
        include
)

target_link_libraries(${PROJECT_NAME}
        INTERFACE
        glm
)

add_subdirectory(tests)
