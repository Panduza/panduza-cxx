cmake_minimum_required(VERSION 3.25)

include(cmake/version.cmake)
get_version_from_changelog()
project(PZACXX_LIBRARY VERSION ${PZACXX_VERSION} LANGUAGES CXX)

set(LIBRARY_NAME pzacxx)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(cmake/tidy.cmake)
include(cmake/format.cmake)
include(cmake/compiler.cmake)
include(cmake/lib.cmake)

find_package(spdlog REQUIRED)
find_package(nlohmann_json REQUIRED)
find_package(PahoMqttCpp REQUIRED)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

add_subdirectory(source)

option(BUILD_EXAMPLES "Build examples" OFF)
if (BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()
