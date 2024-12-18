add_subdirectory(asio)
include(FetchContent)

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