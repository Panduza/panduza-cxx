configure_file(${CMAKE_CURRENT_SOURCE_DIR}/core/version.cxx.in
               ${CMAKE_CURRENT_BINARY_DIR}/version.cxx)

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

    ${CMAKE_CURRENT_BINARY_DIR}/version.cxx
)
