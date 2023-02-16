cmake_minimum_required(VERSION 3.25)

project("Panduza C++ API")
set(LIBRARY_NAME pza-cxx)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

file(STRINGS "${PROJECT_SOURCE_DIR}/CHANGELOG.md" CHANGELOG_LINES REGEX "^# \\[([0-9]+)\\.([0-9]+)\\.([0-9]+)\\]")

string(REGEX MATCH "^# \\[([0-9]+)\\.([0-9]+)\\.([0-9]+)\\]" VERSION_STRING "${CHANGELOG_LINES}")
string(REGEX REPLACE "^# \\[([0-9]+)\\.([0-9]+)\\.([0-9]+)\\].*" "\\1" LIBRARY_VERSION_MAJOR "${VERSION_STRING}")
string(REGEX REPLACE "^# \\[([0-9]+)\\.([0-9]+)\\.([0-9]+)\\].*" "\\2" LIBRARY_VERSION_MINOR "${VERSION_STRING}")
string(REGEX REPLACE "^# \\[([0-9]+)\\.([0-9]+)\\.([0-9]+)\\].*" "\\3" LIBRARY_VERSION_PATCH "${VERSION_STRING}")

set(LIBRARY_VERSION "${LIBRARY_VERSION_MAJOR}.${LIBRARY_VERSION_MINOR}.${LIBRARY_VERSION_PATCH}")

if(NOT LIBRARY_VERSION MATCHES "^[0-9]\\.[0-9]\\.[0-9]$")
    message(FATAL_ERROR "Invalid project version: ${LIBRARY_VERSION}")
endif()

add_compile_options(-Wall -Wextra -Wno-unused-result)

set(SPDLOG_FMT_EXTERNAL 1)

find_package(spdlog REQUIRED)
find_package(nlohmann_json REQUIRED)
find_package(paho-mqtt-cpp REQUIRED)
find_package(GTest REQUIRED)
find_package(cppcheck REQUIRED)

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

configure_file("${CMAKE_SOURCE_DIR}/version.h.in" "${CMAKE_SOURCE_DIR}/source/pza/version.hxx")

enable_testing()

add_subdirectory(source)
add_subdirectory(test)

set(CMAKE_DEBUG_POSTFIX -debug)
set_target_properties(${LIBRARY_NAME}
    PROPERTIES
    VERSION "${LIBRARY_VERSION}"
    SOVERSION "${LIBRARY_VERSION_MAJOR}"
    DEBUG_POSTFIX ${CMAKE_DEBUG_POSTFIX}
)

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