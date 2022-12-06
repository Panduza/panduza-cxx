add_library(${LIBRARY_NAME} SHARED)

target_sources(${LIBRARY_NAME} PRIVATE
    core/Core.cxx
    core/Client.cxx
)

target_include_directories(${LIBRARY_NAME} PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}
)