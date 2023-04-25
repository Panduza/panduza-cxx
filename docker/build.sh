#!/bin/bash

SCRIPT_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )
# DOCKER_FILE=$SCRIPT_DIR/Dockerfile
PROJECT_DIR=$SCRIPT_DIR/..

docker build -t panduza-cxx/builder $SCRIPT_DIR


docker run -v$PROJECT_DIR:/project panduza-cxx/builder

