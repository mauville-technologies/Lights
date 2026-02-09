project(lights_engine)

set(CMAKE_CXX_STANDARD 23)

add_subdirectory(third_party)

find_package(OpenGL REQUIRED)

set(SOURCES
        src/core/algo/graph_node.cpp

        src/core/audio/nodes/audio_cue.cpp
        src/core/audio/nodes/audio_fan_in_mixer_node.cpp
        src/core/audio/nodes/saw_tooth_node.cpp
        src/core/audio/audio_subsystem.cpp
        src/core/audio/audio_graph.cpp

        src/framework/input/input_keys.cpp
        src/framework/input/input_subsystem.cpp

        src/framework/layers/clay/components/clay_panel.cpp
        src/framework/layers/clay/components/clay_canvas_panel.cpp
        src/framework/layers/clay/clay_ui_layer.cpp

        src/core/platform/window.cpp
        src/core/platform/GLFW/glfw_keys.cpp
        src/core/platform/GLFW/glfw_window.cpp
        src/core/platform/SDL3/sdl_keys.cpp
        src/core/platform/SDL3/sdl_window.cpp

        src/core/rendering/renderables/renderable_viewport.cpp
        src/core/rendering/buffer.cpp
        src/core/rendering/material.cpp
        src/core/rendering/render_target.cpp
        src/core/rendering/renderable.cpp
        src/core/rendering/renderer.cpp
        src/core/rendering/shader.cpp
        src/core/rendering/texture.cpp

        src/framework/scene/scene.cpp
        src/framework/scene/scene_layer_manager.cpp
        src/framework/scene/resource_manager.cpp

        src/core/text/font_loader.cpp

        src/core/util/image.cpp
        src/core/util/ring_buffer.cpp
)

add_library(lights_engine STATIC ${SOURCES})

# if set_base_dir(target) is defined, call it
if (DEFINED set_base_dir)
    set_base_dir(${PROJECT_NAME})
endif ()


# Compile definitions
set(COMPILE_DEFINITIONS "")

# if on windows
if (WIN32)
    list(APPEND COMPILE_DEFINITIONS OZZ_PLATFORM_WINDOWS)
elseif (APPLE)
    list(APPEND COMPILE_DEFINITIONS OZZ_PLATFORM_MACOS)
else ()
    list(APPEND COMPILE_DEFINITIONS OZZ_PLATFORM_LINUX)
endif ()

target_include_directories(${PROJECT_NAME}
        PUBLIC
        include ${Tileson_INCLUDE_DIR}
        PRIVATE
        src
)
target_compile_definitions(lights_engine PUBLIC ${COMPILE_DEFINITIONS})
target_link_libraries(lights_engine PUBLIC ${OPENGL_LIBRARIES} third_party ozz_collision ozz_audio)

