set(examples
    bps
)

foreach(example ${examples})
    add_executable(${example} ${example}.cxx)
    set_target_properties(${example} PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/examples)
    set_source_files_properties(${example}.cxx PROPERTIES
        SKIP_LINTING ON
    )
    target_link_libraries(${example} ${LIBRARY_NAME})
endforeach()
