# Panduza C++ Library

User library to develop C++ applications following Panduza API.

## Dependencies

### Build Deps

| Package | Version |
| ------- | ------- |
| conan   | 1.58    |
| cmake   | 3.25.0  |

Library dependencies are managed wih Conan.

To install conan, https://conan.io/downloads.html.

### Library Deps

| Package       | Version      | Runtime  |
| ------------- | ------------ | -------- |
| paho-mqtt-cpp | 1.2.0        | &#10004; |
| nlohmann JSON | 3.11.2       | &#10004; |
| spdlog        | 1.11.0       | &#10004; |
| Google test   | cci.20210126 |          |
| cppcheck      | 2.10         |          |

## Version

The version of the library is fetched from the top line of CHANGELOG.md.

## Build

To build the library in Debug or Release mode:

```
mkdir build
cd build
conan install .. --build=missing -s build_type=<build mode>
cmake -DCMAKE_INSTALL_PREFIX=./install -DCMAKE_BUILD_TYPE=<build mode> -DCMAKE_TOOLCHAIN_FILE=./<build mode>/generators/conan_toolchain.cmake ..
cmake --build .
cmake --install .
```

To build and export the library as a conan package:

```
mkdir build
cd build
conan create .. --build=missing -s build_type=<build mode>
```

## Test

After building the library, you can test it with:

```
cd build
make test
```

## Check

You can use cppcheck to examine the code.

```
cd build
make check
```