
set (SOURCES
    main.cpp
    generator.cpp
)
add_executable(ozz_typegen ${SOURCES})

target_link_libraries(ozz_typegen
    PRIVATE
        ozz_binarypacking
)

# TODO: This is kind of dirty. I'm having a tough time trying to make these things work better
if (COMMAND set_base_dir)
    set_base_dir(ozz_typegen)
endif()

function(generate_ozz_source INPUT_FILE OUTPUT_FILE)
    # Ensure the code generator executable has been set.
    # Get target property
    get_target_property(TYPEGEN_BIN ozz_typegen RUNTIME_OUTPUT_DIRECTORY)

    add_custom_command(
            OUTPUT ${OUTPUT_FILE}
            COMMAND ${TYPEGEN_BIN}/ozz_typegen ${INPUT_FILE} ${OUTPUT_FILE}
            DEPENDS ${INPUT_FILE} ${TYPEGEN_BIN}
            COMMENT "Generating source file ${OUTPUT_FILE} from ${INPUT_FILE}"
            VERBATIM
    )
endfunction()
