cmake_minimum_required(VERSION 3.20)

project(ozz_audio)

set(CMAKE_CXX_STANDARD 20)

add_subdirectory(third_party)

set(SOURCES
        src/audio.cpp
)

add_library(ozz_audio STATIC ${SOURCES})

target_link_libraries(ozz_audio PUBLIC
        ozz_audio_third_party
)

target_include_directories(ozz_audio
        PUBLIC
        include
)