add_library(${LIBRARY_NAME} SHARED)

target_sources(${LIBRARY_NAME} PRIVATE
    core/Core.cxx
    core/Client.cxx
    core/Interface.cxx
    core/Topic.cxx
    utils/Utils.cxx
)

target_include_directories(${LIBRARY_NAME} PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}
    ${mosquittopp_INCLUDE_DIRS}
    ${spdlog_INCLUDE_DIRS}
    ${CMAKE_SOURCE_DIR}/third_party
)

target_link_libraries(${LIBRARY_NAME} PUBLIC
    spdlog
    mosquittopp
)
