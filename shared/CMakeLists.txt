project(Lights_Shared_Lib)

set(CMAKE_CXX_STANDARD 20)

# Set to build shared libraries
set(BUILD_SHARED_LIBS FALSE)

add_subdirectory(third_party)

find_package(OpenGL REQUIRED)

set(SOURCES
    src/platform/window.cpp
)

add_library(Lights_Shared_Lib STATIC ${SOURCES})

# if set_base_dir(target) is defined, call it
if (DEFINED set_base_dir)
    set_base_dir(${PROJECT_NAME})
endif()

target_include_directories(${PROJECT_NAME} PUBLIC include)
target_link_libraries(Lights_Shared_Lib PUBLIC asio spdlog mongocxx_static cryptopp glfw ${OPENGL_LIBRARIES} glad glm)

