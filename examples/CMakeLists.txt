set(examples
    bps
)

foreach(example ${examples})
    add_executable(${example} ${example}.cxx)
    set_target_properties(${example} PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/examples/bin"
    )
    target_link_libraries(${example} ${LIBRARY_NAME})
    if (CMAKE_SYSTEM_NAME MATCHES "Windows" AND BUILD_SHARED_LIBS)
        add_custom_command(TARGET ${example} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${CMAKE_BINARY_DIR}/bin/*.dll
            $<TARGET_FILE_DIR:${example}>
    )
    endif()
endforeach()
