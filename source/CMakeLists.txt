file(GLOB_RECURSE HEADERS ${CMAKE_SOURCE_DIR}/source/*.hxx)

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/version.hxx.in 
               ${CMAKE_CURRENT_BINARY_DIR}/version.hxx)

target_sources(${LIBRARY_NAME}
    PRIVATE

    core/core.cxx
    core/client.cxx
    core/scanner.cxx
    core/device.cxx
    core/interface.cxx
    core/interface_factory.cxx
    core/attribute.cxx

    utils/json_attribute.cxx
    utils/topic.cxx

    interfaces/device.cxx
    interfaces/platform.cxx
    interfaces/meter.cxx
    interfaces/voltmeter.cxx
    interfaces/ammeter.cxx
    interfaces/bps_chan_ctrl.cxx
)

target_include_directories(${LIBRARY_NAME} PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}
    ../include
)

install(TARGETS ${LIBRARY_NAME}
    FILE_SET HEADERS
)
