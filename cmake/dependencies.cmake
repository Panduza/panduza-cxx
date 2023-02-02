cmake_minimum_required(VERSION 3.21)

include(FetchContent)
set(FETCHCONTENT_BASE_DIR ${CMAKE_CURRENT_LIST_DIR}/deps)
set(FETCHCONTENT_QUIET FALSE)

set(SPDLOG_BUILD_SHARED ON CACHE INTERNAL "")
# Fetch and configure spdlog
FetchContent_Declare(spdlog
    GIT_REPOSITORY https://github.com/gabime/spdlog.git
    GIT_TAG        v1.11.0
)
FetchContent_GetProperties(spdlog)
if(NOT spdlog_POPULATED)
    FetchContent_Populate(spdlog)
    add_subdirectory(${spdlog_SOURCE_DIR} ${spdlog_BINARY_DIR})
endif()
unset(SPDLOG_BUILD_SHARED)

# Fetch and configure Google Test
FetchContent_Declare(googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG        v1.13.0
)
FetchContent_MakeAvailable(googletest)

# Fetch and configure nlohmann/json
FetchContent_Declare(nlohmann_json
    GIT_REPOSITORY https://github.com/nlohmann/json.git
    GIT_TAG        v3.11.2
)
FetchContent_GetProperties(nlohmann_json)
if(NOT nlohmann_json_POPULATED)
    FetchContent_Populate(nlohmann_json)
    add_subdirectory(${nlohmann_json_SOURCE_DIR} ${nlohmann_json_BINARY_DIR})
endif()

# Fetch the Paho MQTT C++ library as a subproject
include(FetchContent)
FetchContent_Declare(paho_mqtt
    GIT_REPOSITORY https://github.com/eclipse/paho.mqtt.cpp.git
    GIT_TAG v1.2.0
)
FetchContent_GetProperties(paho_mqtt)
if(NOT paho_mqtt_POPULATED)
    FetchContent_Populate(paho_mqtt)
    add_subdirectory(${paho_mqtt_SOURCE_DIR} ${paho_mqtt_BINARY_DIR})
endif()
FetchContent_MakeAvailable(paho_mqtt)
