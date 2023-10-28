#!/bin/bash

IMAGE_NAME="pzacxx-build-img"
PROJECT_ROOT_DIR="$(dirname "$(realpath "$0")")/.."

TEMP=$(getopt -o u:p:h --long username:,password:,help -n "$0" -- "$@")
if [ $? != 0 ]; then
    echo "Error processing arguments." >&2
    exit 1
fi

eval set -- "$TEMP"

while true; do
    case "$1" in
        -u|--username)
        GITHUB_USER="$2"
        shift 2
        ;;
        -p|--password)
        PASSWORD="$2"
        shift 2
        ;;
        -h|--help)
        echo "Usage: $0 [-u <username>] [-p <password>] [-h]"
        echo "  -u  --username   Github username"
        echo "  -p  --password   Github token"
        echo "  -h  --help       Display this help message"
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

if [ -z "$GITHUB_USER" ]; then
    echo "Username not specified"
    echo "-h or --help for more information"
    exit 1
fi

if [ -z "$PASSWORD" ]; then
    echo "Password not specified"
    echo "-h or --help for more information"
    exit 1
fi


docker login ghcr.io -u $USERNAME -p $PASSWORD || exit 1
docker build -t $IMAGE_NAME $PROJECT_ROOT_DIR/docker || exit 1
docker tag $IMAGE_NAME ghcr.io/panduza/$IMAGE_NAME:latest
docker push ghcr.io/panduza/$IMAGE_NAME:latest
