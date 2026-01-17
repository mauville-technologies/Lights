FetchContent_Declare(
        samplerate_lib
        GIT_REPOSITORY https://github.com/libsndfile/libsamplerate
        GIT_TAG master
)
FetchContent_MakeAvailable(samplerate_lib)

set(LIBS
        samplerate
)

add_library(ozz_audio_third_party INTERFACE)
target_link_libraries(ozz_audio_third_party
        INTERFACE
        ${LIBS}
)