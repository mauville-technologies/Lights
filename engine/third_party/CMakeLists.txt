include(FetchContent)

set(GLFW_BUILD_WAYLAND FALSE)

set(OZZ_WINDOWING_SYSTEM "GLFW" CACHE STRING "Windowing system to use, options: GLFW, SDL3")

FetchContent_Declare(
        spdlog
        GIT_REPOSITORY https://github.com/gabime/spdlog.git
        GIT_TAG v1.15.0
)

FetchContent_MakeAvailable(spdlog)

set(ENABLE_UNINSTALL OFF CACHE BOOL "Disable uninstall target for mongocxx" FORCE)
FetchContent_Declare(
        mongocxx
        GIT_REPOSITORY https://github.com/mongodb/mongo-cxx-driver.git
        GIT_TAG r4.0.0
)

FetchContent_MakeAvailable(mongocxx)

FetchContent_Declare(
        cryptopp_cmake
        GIT_REPOSITORY git@github.com:abdes/cryptopp-cmake.git
        GIT_TAG CRYPTOPP_8_9_0
)

FetchContent_MakeAvailable(cryptopp_cmake)

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
FetchContent_Declare(
        rtaudio_lib
        GIT_REPOSITORY git@github.com:thestk/rtaudio.git
        GIT_TAG 6.0.1
)
FetchContent_MakeAvailable(rtaudio_lib)

add_subdirectory(asio)
add_subdirectory(glad)
add_subdirectory(clay)
add_subdirectory(stb)
add_subdirectory(imgui)
add_subdirectory(tileson)

add_library(third_party INTERFACE)

set(LIBS
        asio
        spdlog
        mongocxx_static
        cryptopp
        glad
        glm
        stb
        clay
        tileson
        freetype
        toml11
        rtaudio
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
