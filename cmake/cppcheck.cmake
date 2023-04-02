# Add a target for cppcheck
find_program(CPPCHECK_EXECUTABLE cppcheck)
if(CPPCHECK_EXECUTABLE)
    # cppcheck igner never used functions
    add_custom_target(check
        ${CPPCHECK_EXECUTABLE} --enable=all --suppress=missingIncludeSystem
        --template "{file}:{line}:{severity}:{id}:{message}"
        -I ${CMAKE_SOURCE_DIR}/source
        ${CMAKE_SOURCE_DIR}/source
        COMMENT "Running Cppcheck static analysis tool"
    )
endif()