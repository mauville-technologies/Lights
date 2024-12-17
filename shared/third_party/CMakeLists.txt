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