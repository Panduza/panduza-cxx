cmake_minimum_required(VERSION 3.25)

project(Examples)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(libpza-cxx REQUIRED)

set(examples
    psu
)

foreach(example ${examples})
    add_executable(${example} ${example}.cxx)
    set_target_properties(${example} PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
    )
    target_link_libraries(${example} PRIVATE libpza-cxx::libpza-cxx)
endforeach()