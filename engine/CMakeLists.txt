project(lights_engine)

set(CMAKE_CXX_STANDARD 23)

# Set to build shared libraries
set(BUILD_SHARED_LIBS FALSE)

add_subdirectory(third_party)

find_package(OpenGL REQUIRED)

set(SOURCES
        src/audio/audio_subsystem.cpp
        src/audio/audio_submix.cpp

        src/game/2D/sprite.cpp
        src/game/game_object.cpp
        src/game/tiled_map.cpp
        src/game/game_world.cpp

        src/input/input_keys.cpp
        src/input/input_subsystem.cpp

        src/library/layers/ui_layer.cpp
        src/library/objects/tilemap.cpp
        src/library/objects/ui/input/button.cpp
        src/library/objects/ui/input/text_input.cpp
        src/library/objects/ui/text/text_label.cpp

        src/platform/window.cpp
        src/platform/GLFW/glfw_keys.cpp
        src/platform/GLFW/glfw_window.cpp
        src/platform/SDL3/sdl_keys.cpp
        src/platform/SDL3/sdl_window.cpp

        src/rendering/buffer.cpp
        src/rendering/material.cpp
        src/rendering/render_pass.cpp
        src/rendering/renderer.cpp
        src/rendering/shader.cpp
        src/rendering/texture.cpp

        src/scene/scene_layer_manager.cpp

        src/text/font_loader.cpp

        src/util/image.cpp
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
target_link_libraries(lights_engine PUBLIC ${OPENGL_LIBRARIES} third_party ozz_collision)

