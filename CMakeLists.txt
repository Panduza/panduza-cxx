cmake_minimum_required(VERSION 3.21)

project("Panduza C++ API")
set(LIBRARY_NAME pza-cxx)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_compile_options(-Wall -Wextra)

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

include(cmake/dependencies.cmake)

enable_testing()

add_subdirectory(source)
add_subdirectory(examples)
add_subdirectory(test)

set(CMAKE_DEBUG_POSTFIX -debug)
set_target_properties(${LIBRARY_NAME} PROPERTIES DEBUG_POSTFIX ${CMAKE_DEBUG_POSTFIX})
#set_target_properties(${LIBRARY_NAME} PROPERTIES VERSION ${VERSION} SOVERSION ${SPDLOG_VERSION_MAJOR}.${SPDLOG_VERSION_MINOR})

# Add a target for cppcheck
find_program(CPPCHECK_EXECUTABLE cppcheck)
if(CPPCHECK_EXECUTABLE)
# cppcheck igner never used functions
add_custom_target(check
${CPPCHECK_EXECUTABLE} --enable=all --inconclusive --suppress=missingIncludeSystem
--template "{file}:{line}:{severity}:{id}:{message}"
-I ${CMAKE_SOURCE_DIR}/source
${CMAKE_SOURCE_DIR}/source
COMMENT "Running Cppcheck static analysis tool"
)


endif()