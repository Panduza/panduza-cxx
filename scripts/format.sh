#!/bin/bash

CLANG_FORMAT_PATH=$(which clang-format)
SOURCE_FOLDERS="source include examples"

PROJECT_DIR=$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)
SOURCE_PATHS=$(echo $SOURCE_FOLDERS | tr " " "\n" | sed "s|^|$PROJECT_DIR/|")
SOURCE_FILES=$(find $SOURCE_PATHS -name "*.h" -o -name "*.c" -o -name "*.cpp" -o -name "*.hpp" -o -name "*.hxx" -o -name "*.cxx")

if [ -z "$CLANG_FORMAT_PATH" ]; then
    echo "clang-format not found. Please install clang-format and add it to your PATH or set the CLANG_FORMAT_PATH environment variable."
    exit 1
fi

# get options for --format and --check
FORMAT_OPTION=false
CHECK_OPTION=false
while [[ $# -gt 0 ]]; do
    key="$1"
    case $key in
        --format)
            FORMAT_OPTION=true
            shift
            shift
            ;;
        --check)
            CHECK_OPTION=true
            shift
            shift
            ;;
        *)
            echo "Unknown option: $1"
            exit 1
            ;;
    esac
done

if [ "$FORMAT_OPTION" == false ] && [ "$CHECK_OPTION" == false ]; then
    echo "No options provided. Please provide either --format or --check."
    exit 1
fi

if [ "$FORMAT_OPTION" == true ]
then
    echo "Formatting code..."
    $CLANG_FORMAT_PATH --style=file -i $SOURCE_FILES
fi

if [ "$CHECK_OPTION" == true ]
then
    echo "Checking code formatting..."
    $CLANG_FORMAT_PATH --style=file -output-replacements-xml $SOURCE_FILES | grep "<replacement " &> /dev/null && echo "Error! Code is not well formatted." && exit 1 || echo "Format OK" && exit 0
fi
