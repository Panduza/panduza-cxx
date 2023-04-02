#!/bin/bash

SCRIPT_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )
BUILD_DIR=$SCRIPT_DIR/../build

mkdir -p $BUILD_DIR
pushd $BUILD_DIR

echo $PWD

BUILD_TYPE="Debug"

if [ "$1" == 'Release' ]
then
	BUILD_TYPE="Release"
fi

conan install .. --build=missing -s build_type=$BUILD_TYPE
conan build ..
#conan export-pkg .. -f
#conan upload "libpza-cxx" -r conan-local -c
popd
