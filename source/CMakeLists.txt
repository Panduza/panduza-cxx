add_library(${LIBRARY_NAME} SHARED)

target_sources(${LIBRARY_NAME} PRIVATE
    core/Core.cxx
    core/Alias.cxx
    core/Client.cxx
    core/Interface.cxx
    core/Topic.cxx
    core/Attribute.cxx
    core/Field.cxx

    interfaces/Psu.cxx

    utils/Utils.cxx
)

set(spdlog_INCLUDE_DIRS
    ${spdlog_SOURCE_DIR}/include
)
set(spdlog_LIB_DIRS
    ${spdlog_BINARY_DIR}
)

target_include_directories(${LIBRARY_NAME} PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}
    ${spdlog_INCLUDE_DIRS}
)

target_link_libraries(${LIBRARY_NAME} spdlog)
target_link_libraries(${LIBRARY_NAME} nlohmann_json)
target_link_libraries(${LIBRARY_NAME} paho-mqttpp3)