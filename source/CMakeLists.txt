file(GLOB_RECURSE HEADERS ${CMAKE_SOURCE_DIR}/source/*.hxx)

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/pza/version.hxx.in 
               ${CMAKE_CURRENT_BINARY_DIR}/pza/version.hxx)

target_sources(${LIBRARY_NAME}
    PRIVATE

    pza/core/core.cxx
    pza/core/client.cxx
    pza/core/device.cxx
    pza/core/device_factory.cxx
    pza/core/interface.cxx
    pza/core/attribute.cxx

    pza/utils/json.cxx
    pza/utils/string.cxx
    pza/utils/topic.cxx

    pza/devices/bps.cxx
    pza/devices/orphan.cxx

    pza/interfaces/meter.cxx
    pza/interfaces/bps_chan_ctrl.cxx
)

target_include_directories(${LIBRARY_NAME} PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}
)

install(TARGETS ${LIBRARY_NAME}
    FILE_SET HEADERS
)
