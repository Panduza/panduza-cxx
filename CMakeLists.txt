cmake_minimum_required(VERSION 3.25)

project("Panduza C++ API")
set(LIBRARY_NAME pza-cxx)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(cmake/version.cmake)
include(cmake/cppcheck.cmake)




if (MSVC)
  set (HAVE_SYS_TYPES_H 1)
  set (HAVE_STDINT_H    1)
  set (HAVE_STDDEF_H    1) # used by CheckTypeSize module
  set (HAVE_INTTYPES_H  0)
  set (HAVE_UNISTD_H    0)
  set (HAVE_SYS_STAT_H  1)
  set (HAVE_SHLWAPI_H   1)
endif ()




set(SPDLOG_FMT_EXTERNAL 1)
find_package(spdlog REQUIRED)
find_package(nlohmann_json REQUIRED)
find_package(PahoMqttCpp REQUIRED)
if(BUILD_TESTS)
    find_package(GTest REQUIRED)
    find_package(cppcheck REQUIRED)
    add_compile_options(-Wall -Wextra)
endif()

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

configure_file("${CMAKE_SOURCE_DIR}/version.h.in" "${CMAKE_SOURCE_DIR}/source/pza/version.hxx")

add_subdirectory(source)
if(BUILD_TESTS)
    add_subdirectory(test)
endif()
if(BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()

set(CMAKE_DEBUG_POSTFIX -debug)
set_target_properties(${LIBRARY_NAME}
    PROPERTIES
    VERSION "${LIBRARY_VERSION}"
    SOVERSION "${LIBRARY_VERSION_MAJOR}"
    DEBUG_POSTFIX ${CMAKE_DEBUG_POSTFIX}
)

include(InstallRequiredSystemLibraries)
set(CPACK_DEBIAN_PACKAGE_NAME "pza-cxx")
set(CPACK_DEBIAN_FILE_NAME "DEB-DEFAULT")
set(CPACK_RPM_FILE_NAME "RPM-DEFAULT")
set(CPACK_DEBIAN_PACKAGE_DEPENDS "libspdlog-dev, nlohmann-json3-dev, libpaho-mqtt-dev")
set(CPACK_STRIP_FILES YES)
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE")
set(CPACK_DEBIAN_PACKAGE_VERSION "0.1.0")
set(CPACK_DEBIAN_PACKAGE_MAINTAINER "Panduza Corp (lmao)")
include(CPack)
