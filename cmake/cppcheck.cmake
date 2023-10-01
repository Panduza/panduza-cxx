if (CMAKE_SYSTEM_NAME MATCHES "Linux")
    find_program(CPPCHECK_EXECUTABLE cppcheck)
    if(CPPCHECK_EXECUTABLE)
        add_custom_target(check
            ${CPPCHECK_EXECUTABLE} --enable=all --suppress=missingIncludeSystem --suppress=unusedFunction
            --template "{file}:{line}:{severity}:{id}:{message}"
            -I ${CMAKE_SOURCE_DIR}/source
            ${CMAKE_SOURCE_DIR}/source
            COMMENT "Running Cppcheck static analysis tool"
        )
    else()
        message(WARNING "Cppcheck not found, can't run static analysis")
    endif()
endif()
