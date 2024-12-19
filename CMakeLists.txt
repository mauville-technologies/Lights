cmake_minimum_required(VERSION 3.29)
project(Lights)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/dist/${CMAKE_SYSTEM_NAME}/${CMAKE_BUILD_TYPE})

# if release build
if (CMAKE_BUILD_TYPE MATCHES "Release")
    # Set optimization flags for different compilers
    if (CMAKE_CXX_COMPILER_ID MATCHES "Clang" OR CMAKE_CXX_COMPILER_ID MATCHES "GNU")
        set(CMAKE_CXX_FLAGS_RELEASE "-O3  -flto -ffunction-sections -fdata-sections -DNDEBUG -s")
        set(CMAKE_C_FLAGS_RELEASE "-O3 -flto -ffunction-sections -fdata-sections -DNDEBUG -s")
    elseif (CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
        set(CMAKE_CXX_FLAGS_RELEASE "/O2 /GL /Gy /DNDEBUG")
        set(CMAKE_SHARED_LINKER_FLAGS_RELEASE "${CMAKE_SHARED_LINKER_FLAGS_RELEASE} /OPT:REF /OPT:ICF /DEBUG:NONE")
        set(CMAKE_C_FLAGS_RELEASE "/O2 /GL /Gy /DNDEBUG")
    endif()
endif()

add_subdirectory(shared)
add_subdirectory(server)
add_subdirectory(mod_client)

add_subdirectory(cli_tool)
