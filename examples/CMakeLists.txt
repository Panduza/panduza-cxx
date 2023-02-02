
set(examples
    psu
)

foreach(example ${examples})
    add_executable(${example} ${example}.cxx)
    set_target_properties(${example} PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/examples"
    )
    target_link_libraries(${example} ${LIBRARY_NAME})
endforeach()