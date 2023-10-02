#!/bin/bash

TARGET="Linux"
LIB_MODE="Shared"
BUILD_MODE="Debug"
PROJECT_ROOT_DIR="$(dirname "$(realpath "$0")")/.."
CONAN_PROFILES_DIR="$PROJECT_ROOT_DIR/conan_profiles"
EXAMPLES="ON"

TEMP=$(getopt -o t:l:b:h --long target:,lib:,build:,help,no-examples -n "$0" -- "$@")
if [ $? != 0 ]; then
    echo "Error processing arguments." >&2
    exit 1
fi

eval set -- "$TEMP"

while true; do
    case "$1" in
        -t|--target)
        TARGET="$(tr '[:lower:]' '[:upper:]' <<< ${2:0:1})${2:1}"
        shift 2
        ;;
        -l|--lib)
        LIB_MODE="$(tr '[:lower:]' '[:upper:]' <<< ${2:0:1})${2:1}"
        shift 2
        ;;
        -b|--build)
        BUILD_MODE="$(tr '[:lower:]' '[:upper:]' <<< ${2:0:1})${2:1}"
        shift 2
        ;;
		--no-examples)
		EXAMPLES="OFF"
		shift
		;;
        -h|--help)
        echo "Usage: $0 [-t <target>] [-l <lib>] [-b <build>] [-h]"
        echo "  -t  --target   Target platform (Windows or Linux). Default is Linux"
        echo "  -l  --lib      Library mode (Static or Shared). Default is Shared"
        echo "  -b  --build    Build mode (Debug or Release). Default is Debug"
        echo "  -h  --help     Display this help message"
        exit 0
        ;;
        --)
        shift
        break
        ;;
        *)
        echo "Unknown option: $1"
        exit 1
        ;;
    esac
done

if [ "$TARGET" == "Linux" ]; then
    BUILD_DIR="build"
    PROFILE_BUILD="x86_64_Linux"
    PROFILE_HOST="x86_64_Linux"
elif [ "$TARGET" == "Windows" ]; then
    BUILD_DIR="buildwin"
    PROFILE_BUILD="x86_64_Linux"
    PROFILE_HOST="x86_64_Cross_Windows"
else
    echo "Target not supported: $TARGET"
    exit 1
fi

if [ "$LIB_MODE" == "Static" ]; then
    BUILD_DIR="${BUILD_DIR}_static"
elif [ "$LIB_MODE" == "Shared" ]; then
	continue
else
	echo "Library mode not supported: $LIB_MODE"
	exit 1
fi

FULL_BUILD_DIR="$PROJECT_ROOT_DIR/$BUILD_DIR"
FULL_PROFILE_BUILD="$CONAN_PROFILES_DIR/$PROFILE_BUILD"
FULL_PROFILE_HOST="$CONAN_PROFILES_DIR/$PROFILE_HOST"

echo "Configuration:"
echo "  Target     : $TARGET"
echo "  Lib Mode   : $LIB_MODE"
echo "  Build Mode : $BUILD_MODE"

if [ ! -d "$FULL_BUILD_DIR" ]; then
	echo "Build directory not found: $FULL_BUILD_DIR"
	echo "Please run install_dependencies.sh first"
	exit 1
fi

cd $FULL_BUILD_DIR
cmake \
	-DCMAKE_TOOLCHAIN_FILE=./$BUILD_MODE/generators/pzacxx_toolchain.cmake \
	-DCMAKE_BUILD_TYPE=$BUILD_MODE \
	-DBUILD_EXAMPLES=$EXAMPLES \
	..
cmake --build . --config $BUILD_MODE --parallel ${nproc}
