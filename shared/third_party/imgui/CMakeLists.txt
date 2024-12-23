project(imgui)

include(FetchContent)

# FetchContent and put all files into ./imgui_src
FetchContent_Declare(
        imgui
        GIT_REPOSITORY https://github.com/ocornut/imgui.git
        GIT_TAG docking
)
FetchContent_MakeAvailable(imgui)

# glob all cpp files in imgui_lib
set(IMGUI_SOURCES
        ${imgui_SOURCE_DIR}/imgui.cpp
        ${imgui_SOURCE_DIR}/imgui_demo.cpp
        ${imgui_SOURCE_DIR}/imgui_draw.cpp
        ${imgui_SOURCE_DIR}/imgui_widgets.cpp
        ${imgui_SOURCE_DIR}/imgui_tables.cpp
        ${imgui_SOURCE_DIR}/misc/cpp/imgui_stdlib.cpp
        ${imgui_SOURCE_DIR}/backends/imgui_impl_glfw.cpp
        ${imgui_SOURCE_DIR}/backends/imgui_impl_opengl3.cpp
)

# Create a library from the source files
add_library(${PROJECT_NAME} STATIC ${IMGUI_SOURCES})

# Include the imgui_lib directory
target_include_directories(${PROJECT_NAME} PUBLIC ${imgui_SOURCE_DIR})

target_link_libraries(${PROJECT_NAME} glfw)