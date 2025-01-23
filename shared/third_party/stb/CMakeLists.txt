project(stb)

set(SOURCES
    image.cpp
)

add_library(${PROJECT_NAME} STATIC ${SOURCES})

target_include_directories(
    ${PROJECT_NAME}
    PUBLIC
        stb/
)

