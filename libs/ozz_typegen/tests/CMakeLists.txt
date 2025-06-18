set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# GOOGLE TEST STUFF
include(FetchContent)
FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/refs/tags/v1.17.0.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

# Generate sources stuff
file (GLOB DEFINITION_FILES "${CMAKE_CURRENT_SOURCE_DIR}/test_types/defs/*.ozz")
set(GENERATED_DIR "${CMAKE_CURRENT_SOURCE_DIR}/test_types")
file(MAKE_DIRECTORY ${GENERATED_DIR})

set(GENERATED_SOURCES "")

foreach(DEF_FILE IN LISTS DEFINITION_FILES)
    get_filename_component(DEF_NAME ${DEF_FILE} NAME_WE)
    set(OUTPUT_FILE "${GENERATED_DIR}/${DEF_NAME}.generated.h")
    list (APPEND GENERATED_SOURCES ${OUTPUT_FILE})

    generate_ozz_source(${DEF_FILE} ${OUTPUT_FILE})
endforeach()

add_custom_target(generate_sources ALL DEPENDS ${GENERATED_SOURCES})
add_dependencies(generate_sources ozz_typegen)


# Exec stuff
add_executable(ozz_typegen_tests example.cpp ${GENERATED_SOURCES})
add_dependencies(ozz_typegen_tests generate_sources)
target_link_libraries(ozz_typegen_tests PRIVATE gtest gtest_main ozz_binarypacking)

include(GoogleTest)
gtest_discover_tests(ozz_typegen_tests)
if (COMMAND set_base_dir)
    set_base_dir(ozz_typegen_tests)
endif()
