include_guard()

add_library(${LIBRARY_NAME})

target_link_libraries(${LIBRARY_NAME}
  $<$<BOOL:${BUILD_SHARED_LIBS}>:PahoMqttCpp::paho-mqttpp3>
  $<$<NOT:$<BOOL:${BUILD_SHARED_LIBS}>>:PahoMqttCpp::paho-mqttpp3-static>
  spdlog::spdlog
  nlohmann_json::nlohmann_json
)

target_include_directories(${LIBRARY_NAME} PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

set_target_properties(${LIBRARY_NAME} PROPERTIES
    VERSION ${PROJECT_VERSION}
    SOVERSION ${PROJECT_VERSION_MAJOR}
)

set_target_properties(${LIBRARY_NAME} PROPERTIES DEBUG_POSTFIX d)

install(TARGETS ${LIBRARY_NAME}
    EXPORT ${PROJECT_NAME}Targets
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    RUNTIME DESTINATION bin
)

install(DIRECTORY include/ DESTINATION include)
