include_guard()

include(GoogleTest)

find_package(GTest REQUIRED)

add_executable(unit_tests
    unit/core.cxx
    unit/client.cxx
    unit/attribute.cxx
)

target_link_libraries(unit_tests
    GTest::gtest_main
    ${LIBRARY_NAME}
)

gtest_discover_tests(unit_tests)
