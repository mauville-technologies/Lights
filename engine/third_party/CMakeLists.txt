include(FetchContent)

set(GLFW_BUILD_WAYLAND FALSE)

set(OZZ_WINDOWING_SYSTEM "GLFW" CACHE STRING "Windowing system to use, options: GLFW, SDL3")
set(BUILD_SHARED_LIBS OFF CACHE BOOL "Build all libraries as static" FORCE)
FetchContent_Declare(
        spdlog
        GIT_REPOSITORY https://github.com/gabime/spdlog.git
        GIT_TAG v1.15.0
)

FetchContent_MakeAvailable(spdlog)

FetchContent_Declare(
        glmlib
        GIT_REPOSITORY git@github.com:g-truc/glm.git
        GIT_TAG 1.0.1
)

FetchContent_MakeAvailable(glmlib)

FetchContent_Declare(
        freetype2_lib
        GIT_REPOSITORY https://gitlab.freedesktop.org/freetype/freetype.git
        GIT_TAG VER-2-13-3
)
FetchContent_MakeAvailable(freetype2_lib)

FetchContent_Declare(
        toml11_lib
        GIT_REPOSITORY https://github.com/ToruNiina/toml11.git
        GIT_TAG v4.4.0
)
FetchContent_MakeAvailable(toml11_lib)

set(RTAUDIO_TARGETNAME_UNINSTALL "rtaudio_uninstall" CACHE STRING "The RTAUDIO uninstall target name" FORCE)

# if on linux
if (CMAKE_SYSTEM_NAME STREQUAL "Linux")
    set(RTAUDIO_API_ALSA OFF)
    set(RTAUDIO_API_PULSE ON)
endif ()

FetchContent_Declare(
        rtaudio_lib
        GIT_REPOSITORY git@github.com:thestk/rtaudio.git
        GIT_TAG 6.0.1
)
FetchContent_MakeAvailable(rtaudio_lib)

FetchContent_Declare(
        EnTT_lib
        GIT_REPOSITORY https://github.com/skypjack/entt.git
        GIT_TAG v3.16.0
)

FetchContent_MakeAvailable(EnTT_lib)

set(RAPIDJSON_BUILD_TESTS OFF CACHE BOOL "" FORCE)
FetchContent_Declare(
        nlohmann_lib
        GIT_REPOSITORY https://github.com/nlohmann/json.git
        GIT_TAG v3.12.0
)

FetchContent_MakeAvailable(nlohmann_lib)

#add_subdirectory(asio)
add_subdirectory(glad)
add_subdirectory(clay)
add_subdirectory(stb)

add_library(third_party INTERFACE)

set(LIBS
        #        asio
        spdlog
        glad
        glm
        stb
        clay
        freetype
        toml11
        rtaudio
        EnTT::EnTT
        nlohmann_json
)

set(THIRD_PARTY_COMPILE_DEFS "")
# if window system is GLFW, add glfw
if (OZZ_WINDOWING_SYSTEM STREQUAL "GLFW")
    FetchContent_Declare(
            glfwlib
            GIT_REPOSITORY git@github.com:glfw/glfw.git
            GIT_TAG 3.4
    )

    FetchContent_MakeAvailable(glfwlib)
    list(APPEND LIBS glfw)
    list(APPEND THIRD_PARTY_COMPILE_DEFS OZZ_GLFW)

elseif (OZZ_WINDOWING_SYSTEM STREQUAL "SDL3")
    # not implemented error
    #    message(FATAL_ERROR "SDL3 windowing system is not implemented yet.")
    FetchContent_Declare(
            SDL_Lib
            GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
            GIT_TAG release-3.2.16
    )
    FetchContent_MakeAvailable(SDL_Lib)
    list(APPEND LIBS SDL3::SDL3)
    list(APPEND THIRD_PARTY_COMPILE_DEFS OZZ_SDL3)
endif ()

target_link_libraries(third_party INTERFACE ${LIBS})

set_target_properties(third_party PROPERTIES INTERFACE_SYSTEM_INCLUDE_DIRECTORIES $<TARGET_PROPERTY:third_party,INTERFACE_INCLUDE_DIRECTORIES>)
target_compile_options(third_party INTERFACE "-w")
target_compile_definitions(third_party INTERFACE ${THIRD_PARTY_COMPILE_DEFS})
