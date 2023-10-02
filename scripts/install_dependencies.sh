#!/bin/bash

TARGET="Linux"
LIB_MODE="Shared"
BUILD_MODE="Debug"
PROJECT_ROOT_DIR="$(dirname "$(realpath "$0")")/.."
CONAN_PROFILES_DIR="$PROJECT_ROOT_DIR/conan_profiles"

TEMP=$(getopt -o t:l:b:h --long target:,lib:,build:,help -n "$0" -- "$@")
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
fi

FULL_BUILD_DIR="$PROJECT_ROOT_DIR/$BUILD_DIR"
FULL_PROFILE_BUILD="$CONAN_PROFILES_DIR/$PROFILE_BUILD"
FULL_PROFILE_HOST="$CONAN_PROFILES_DIR/$PROFILE_HOST"

echo "Configuration:"
echo "  Target     : $TARGET"
echo "  Lib Mode   : $LIB_MODE"
echo "  Build Mode : $BUILD_MODE"

mkdir -p $FULL_BUILD_DIR
cd $FULL_BUILD_DIR
conan install \
    -s build_type=$BUILD_MODE \
    -o shared=$( [ "$LIB_MODE" == "Shared" ] && echo "True" || echo "False" ) \
    --build=missing \
    --profile:b $FULL_PROFILE_BUILD \
    --profile:h $FULL_PROFILE_HOST \
    --install-folder=$FULL_BUILD_DIR \
    $PROJECT_ROOT_DIR
