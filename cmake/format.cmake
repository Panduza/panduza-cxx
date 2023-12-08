find_program(CLANG_FORMAT "clang-format")

file(GLOB_RECURSE ALL_SOURCE_FILES 
  ${CMAKE_SOURCE_DIR}/source/*.cxx
  ${CMAKE_SOURCE_DIR}/source/*.hxx
  ${CMAKE_SOURCE_DIR}/include/*.hxx
  ${CMAKE_SOURCE_DIR}/examples/*.cxx
  ${CMAKE_SOURCE_DIR}/examples/*.hxx
)

add_custom_target(format
    COMMAND ${CLANG_FORMAT} --style=file -i ${ALL_SOURCE_FILES}
    COMMENT "Running clang-format on all source files"
)

add_custom_target(format-check
    COMMAND ${CLANG_FORMAT} --style=file -output-replacements-xml ${ALL_SOURCE_FILES} | grep \"<replacement \" && echo \"check-format failed. Run cmake --build <build> format\" && exit 1 || exit 0
    COMMENT "Checking code format"
)
