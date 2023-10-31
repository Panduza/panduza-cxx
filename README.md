# Panduza C++ Library

User library to develop C++ applications following Panduza API.

## Dependencies

### Build Deps

| Package | Version  |
| ------- | -------- |
| GCC     | 13       |
| conan   | 1.60     |
| cmake   | >=3.25.0 |

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

## Build

```
./scripts/install_dependencies.sh
./scripts/build.sh
```

```
$ ./scripts/install_dependencies.sh --help
Usage: ./scripts/install_dependencies.sh [-t <target>] [-l <lib>] [-b <build>] [-h]
  -t  --target   Target platform (Windows or Linux). Default is Linux
  -l  --lib      Library mode (Static or Shared). Default is Shared
  -b  --build    Build mode (Debug or Release). Default is Debug
  -h  --help     Display this help message
```
```
$ ./scripts/build.sh --help
Usage: ./scripts/build.sh [-t <target>] [-l <lib>] [-b <build>] [-h]
  -t  --target   Target platform (Windows or Linux). Default is Linux
  -l  --lib      Library mode (Static or Shared). Default is Shared
  -b  --build    Build mode (Debug or Release). Default is Debug
  -h  --help     Display this help message
```
