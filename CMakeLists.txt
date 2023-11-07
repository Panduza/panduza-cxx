cmake_minimum_required(VERSION 3.25)

project(PZACXX VERSION 0.1.0)
set(LIBRARY_NAME pza-cxx)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(cmake/cppcheck.cmake)

add_compile_options(-Wall -Wextra)

set(SPDLOG_FMT_EXTERNAL 1)
find_package(spdlog REQUIRED)
find_package(nlohmann_json REQUIRED)
find_package(PahoMqttCpp REQUIRED)

if (CMAKE_SYSTEM_NAME MATCHES "Linux")
    find_package(GTest REQUIRED)
    find_package(cppcheck REQUIRED)
endif()

if (CMAKE_SYSTEM_NAME MATCHES "Windows")
	set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -Wl,--allow-multiple-definition")
	set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,--allow-multiple-definition")
elseif (CMAKE_SYSTEM_NAME MATCHES "Linux" AND NOT BUILD_SHARED_LIBS)
	set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,--allow-multiple-definition")
endif()

if (CMAKE_SYSTEM_NAME MATCHES "Windows" AND NOT BUILD_SHARED_LIBS)
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static")
endif()

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
add_library(${LIBRARY_NAME})

add_subdirectory(source)
#add_subdirectory(test)

option(BUILD_EXAMPLES "Build examples" OFF)
if(BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()

target_include_directories(${LIBRARY_NAME} PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}
    ${CMAKE_BINARY_DIR}
)

target_link_libraries(${LIBRARY_NAME}
  $<$<BOOL:${BUILD_SHARED_LIBS}>:PahoMqttCpp::paho-mqttpp3>
  $<$<NOT:$<BOOL:${BUILD_SHARED_LIBS}>>:PahoMqttCpp::paho-mqttpp3-static>
  spdlog::spdlog
  nlohmann_json::nlohmann_json
)

if (CMAKE_SYSTEM_NAME MATCHES "Windows" AND BUILD_SHARED_LIBS)
    message(STATUS "Copying DLLs from ${CMAKE_BINARY_DIR}/${CMAKE_BUILD_TYPE}/bin to ${CMAKE_BINARY_DIR}/bin")
    add_custom_command(TARGET ${LIBRARY_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${CMAKE_BINARY_DIR}/${CMAKE_BUILD_TYPE}/bin/*.dll
        $<TARGET_FILE_DIR:${LIBRARY_NAME}>
    )
endif()

set_target_properties(${LIBRARY_NAME} PROPERTIES
    VERSION ${PROJECT_VERSION}
    SOVERSION ${PROJECT_VERSION_MAJOR}
)
