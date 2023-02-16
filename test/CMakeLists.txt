set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
set(BUILD_GMOCK OFF CACHE BOOL "" FORCE)

include(GoogleTest)

add_executable(unitest
    main.cxx
    connection.cxx
    alias.cxx
    interface.cxx
    psu.cxx
)

set_target_properties(unitest PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/test"
)

target_link_libraries(unitest ${LIBRARY_NAME} GTest::GTest)

gtest_discover_tests(unitest
    DISCOVERY_TIMEOUT 60
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/test
)