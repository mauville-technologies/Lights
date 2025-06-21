cmake_minimum_required(VERSION 3.29)
project(Lights)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(OZZ_OUTPUT_BASE_DIR ${CMAKE_SOURCE_DIR}/dist/${CMAKE_SYSTEM_NAME}/${CMAKE_BUILD_TYPE})

## function for setting base directory
function(set_base_dir target)
    message(STATUS "Setting base directory for ${target} to ${OZZ_OUTPUT_BASE_DIR}/${target}")
    set_target_properties(${target} PROPERTIES
            RUNTIME_OUTPUT_DIRECTORY ${OZZ_OUTPUT_BASE_DIR}/${target}
            ARCHIVE_OUTPUT_DIRECTORY ${OZZ_OUTPUT_BASE_DIR}/${target}
    )
endfunction()

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

# if debug build define OZZ_DEBUG, it needs to affect ifdefs in the code
if (CMAKE_BUILD_TYPE MATCHES "Debug")
    add_definitions(-DOZZ_DEBUG)
endif()


# The shared library is built first as it exposes functionality and third party libs to be used by other things
add_subdirectory(shared)

add_subdirectory(libs/ozz_binarypacking)
add_subdirectory(libs/ozz_collision)
add_subdirectory(libs/ozz_typegen)
add_subdirectory(server)
add_subdirectory(client)

add_subdirectory(cli_tool)
