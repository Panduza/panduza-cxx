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
cmake -DCMAKE_INSTALL_PREFIX=./install -DCMAKE_BUILD_TYPE=$BUILD_TYPE -DCMAKE_TOOLCHAIN_FILE=./$BUILD_TYPE/generators/conan_toolchain.cmake ..
cmake --build . --parallel ${nproc}
cmake --install .
popd
