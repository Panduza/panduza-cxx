cmake_minimum_required(VERSION 3.25)

include(cmake/version.cmake)
get_version_from_changelog()
project(PZACXX_LIBRARY VERSION ${PZACXX_VERSION_MAJOR}.${PZACXX_VERSION_MINOR}.${PZACXX_VERSION_PATCH} LANGUAGES CXX)

set(LIBRARY_NAME pzacxx)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(spdlog REQUIRED)
find_package(nlohmann_json REQUIRED)
find_package(PahoMqttCpp REQUIRED)
if (CMAKE_SYSTEM_NAME MATCHES "Linux")
    find_package(GTest REQUIRED)
    find_package(cppcheck REQUIRED)
endif()

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

include(cmake/compiler.cmake)
include(cmake/lib.cmake)
include(cmake/cppcheck.cmake)

add_subdirectory(source)
#add_subdirectory(test)

option(BUILD_EXAMPLES "Build examples" OFF)
if (BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()

#if (CMAKE_SYSTEM_NAME MATCHES "Windows" AND BUILD_SHARED_LIBS)
#    message(STATUS "Copying DLLs from ${CMAKE_BINARY_DIR}/${CMAKE_BUILD_TYPE}/bin to ${CMAKE_BINARY_DIR}/bin")
#    add_custom_command(TARGET ${LIBRARY_NAME} POST_BUILD
#        COMMAND ${CMAKE_COMMAND} -E copy_if_different
#        ${CMAKE_BINARY_DIR}/${CMAKE_BUILD_TYPE}/bin/*.dll
#        $<TARGET_FILE_DIR:${LIBRARY_NAME}>
#    )
#endif()

set_target_properties(${LIBRARY_NAME} PROPERTIES
    VERSION ${PROJECT_VERSION}
    SOVERSION ${PROJECT_VERSION_MAJOR}
)
