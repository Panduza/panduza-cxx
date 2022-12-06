#!/bin/bash

rm -rf build
mkdir -p build
pushd build
cmake ..
cmake --build .
popd
