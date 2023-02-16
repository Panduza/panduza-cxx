add_library(${LIBRARY_NAME})

file(GLOB_RECURSE HEADERS ${CMAKE_SOURCE_DIR}/source/*.hxx)

target_sources(${LIBRARY_NAME}
    PRIVATE
    
    pza/core/Core.cxx
    pza/core/Alias.cxx
    pza/core/Client.cxx
    pza/core/Interface.cxx
    pza/core/Topic.cxx
    pza/core/Attribute.cxx
    pza/core/Field.cxx
    pza/interfaces/Psu.cxx
    pza/utils/Utils.cxx

    PUBLIC

    FILE_SET HEADERS
    FILES  ${HEADERS}
)

target_include_directories(${LIBRARY_NAME} PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}
    ${CMAKE_BINARY_DIR}
)

target_link_libraries(${LIBRARY_NAME}
    PUBLIC
    spdlog::spdlog
    nlohmann_json::nlohmann_json
    paho-mqtt-cpp::paho-mqtt-cpp
)

install(TARGETS ${LIBRARY_NAME}
    FILE_SET HEADERS
)