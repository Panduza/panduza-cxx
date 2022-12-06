cmake_minimum_required(VERSION 3.21)

project("Panduza C++ API")
set(VERSION 0.0.0)
set(LIBRARY_NAME pza-cxx)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(DOCUMENTATION OFF CACHE INTERNAL "")
set(WITH_CJSON OFF CACHE INTERNAL "")
include(FetchContent)
set(FETCHCONTENT_BASE_DIR ${CMAKE_CURRENT_LIST_DIR}/cmake/deps)
set(FETCHCONTENT_QUIET FALSE)
include(${CMAKE_CURRENT_LIST_DIR}/cmake/dependencies.cmake)
FetchContent_MakeAvailable(mosquittopp)
unset(DOCUMENTATION)
unset(WITH_CJSON)

add_definitions(-DVERSION="${VERSION}")

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

add_subdirectory(source)
add_subdirectory(examples)