#!/bin/bash

rm -rf build/bin/test
mkdir -p build
pushd build
if [ "$1" ] && [ $1 -eq "1" ]
then
	cmake ..
else
	cmake -DCMAKE_BUILD_TYPE=Debug ..
fi
cmake --build . --parallel ${nproc}
popd
