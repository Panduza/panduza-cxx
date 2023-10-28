include_guard()

function(get_version_from_changelog)
    file(STRINGS CHANGELOG.md FIRST_LINE LIMIT_COUNT 1)

    string(REGEX REPLACE "# \\[([0-9]+)\\.([0-9]+)\\.([0-9]+)\\].*" "\\1;\\2;\\3" VERSION_NUMBERS "${FIRST_LINE}")
    list(GET VERSION_NUMBERS 0 MAJOR)
    list(GET VERSION_NUMBERS 1 MINOR)
    list(GET VERSION_NUMBERS 2 PATCH)

    set(PZACXX_VERSION_MAJOR ${MAJOR} PARENT_SCOPE)
    set(PZACXX_VERSION_MINOR ${MINOR} PARENT_SCOPE)
    set(PZACXX_VERSION_PATCH ${PATCH} PARENT_SCOPE)
    set(PZACXX_VERSION "${MAJOR}.${MINOR}.${PATCH}" PARENT_SCOPE)

    find_package(Git REQUIRED)
    execute_process(
        COMMAND ${GIT_EXECUTABLE} rev-parse HEAD
        RESULT_VARIABLE result
        OUTPUT_VARIABLE GITHASH
        OUTPUT_STRIP_TRAILING_WHITESPACE
    )
    if (result)
        message(FATAL_ERROR "Failed to get git hash: ${result}")
    endif()
    set(PZACXX_VERSION_GITHASH ${GITHASH} PARENT_SCOPE)
endfunction()
