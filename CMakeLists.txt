cmake_minimum_required(VERSION 3.21)

project("Panduza C++ API")
set(VERSION 0.0.0)
set(LIBRARY_NAME pza-cxx)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


include(FetchContent)
set(FETCHCONTENT_BASE_DIR ${CMAKE_CURRENT_LIST_DIR}/cmake/deps)
set(FETCHCONTENT_QUIET FALSE)
include(${CMAKE_CURRENT_LIST_DIR}/cmake/dependencies.cmake)

set(DOCUMENTATION OFF CACHE INTERNAL "")
set(WITH_CJSON OFF CACHE INTERNAL "")
FetchContent_MakeAvailable(mosquittopp)
unset(DOCUMENTATION)
unset(WITH_CJSON)

set(SPDLOG_BUILD_SHARED ON CACHE INTERNAL "")
FetchContent_MakeAvailable(spdlog)
unset(SPDLOG_BUILD_SHARED)

FetchContent_MakeAvailable(json)

set(mosquittopp_INCLUDE_DIRS
    ${mosquitto_SOURCE_DIR}/include
    ${mosquitto_SOURCE_DIR}/lib/cpp
)

message(STATUS ${mosquittopp_INCLUDE_DIRS})

set(mosquittopp_LIB_DIRS
    ${mosquittopp_BINARY_DIR}/lib/cpp/
)

set(spdlog_INCLUDE_DIRS
    ${spdlog_SOURCE_DIR}/include
)

set(spdlog_LIB_DIRS
    ${spdlog_BINARY_DIR}
)

set(spdlog_INCLUDE_DIRS
    ${json_SOURCE_DIR}/include
)

add_compile_options(-Wall -Wextra)

add_definitions(-DVERSION="${VERSION}")

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

add_subdirectory(source)
add_subdirectory(examples)

set(CMAKE_DEBUG_POSTFIX -debug)
set_target_properties(${LIBRARY_NAME} PROPERTIES DEBUG_POSTFIX ${CMAKE_DEBUG_POSTFIX})
#set_target_properties(${LIBRARY_NAME} PROPERTIES VERSION ${VERSION} SOVERSION ${SPDLOG_VERSION_MAJOR}.${SPDLOG_VERSION_MINOR})