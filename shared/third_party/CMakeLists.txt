include(FetchContent)

set(GLFW_BUILD_WAYLAND FALSE)

FetchContent_Declare(
    spdlog
    GIT_REPOSITORY https://github.com/gabime/spdlog.git
    GIT_TAG        v1.15.0
)

FetchContent_MakeAvailable(spdlog)

FetchContent_Declare(
    mongocxx
    GIT_REPOSITORY https://github.com/mongodb/mongo-cxx-driver.git
    GIT_TAG        r4.0.0
)

FetchContent_MakeAvailable(mongocxx)


FetchContent_Declare(
    cryptopp_cmake
    GIT_REPOSITORY git@github.com:abdes/cryptopp-cmake.git
    GIT_TAG CRYPTOPP_8_9_0
)

FetchContent_MakeAvailable(cryptopp_cmake)

FetchContent_Declare(
    glfwlib
    GIT_REPOSITORY git@github.com:glfw/glfw.git
    GIT_TAG 3.4
)

FetchContent_MakeAvailable(glfwlib)

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

add_subdirectory(asio)
add_subdirectory(glad)
add_subdirectory(clay)
add_subdirectory(stb)
add_subdirectory(imgui)
add_subdirectory(tileson)

add_library(third_party INTERFACE)
target_link_libraries(third_party INTERFACE asio spdlog mongocxx_static asio spdlog mongocxx_static cryptopp glfw glad glm stb clay imgui tileson freetype)
set_target_properties(third_party PROPERTIES INTERFACE_SYSTEM_INCLUDE_DIRECTORIES $<TARGET_PROPERTY:third_party,INTERFACE_INCLUDE_DIRECTORIES>)
target_compile_options(third_party INTERFACE "-w")
