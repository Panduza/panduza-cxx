#!/bin/bash

CPP_IMG_DEP="pza-cpp-img"
IMAGE_NAME="pza-libcxx-img"
CONTAINER_NAME="pza-libcxx"

IMAGE=$(docker images -q $CPP_IMG_DEP)
if [ -z "$IMAGE" ]; then
  echo "Docker image $CPP_IMG_DEP not found. You need to build the $CPP_IMG_DEP image first."
  exit 1
fi

# Check if the Docker image exists
IMAGE=$(docker images -q $IMAGE_NAME)
if [ -z "$IMAGE" ]; then
  echo "Docker image not found. Building image from Dockerfile..."
  docker build --no-cache --build-arg USER_ID=$(id -u) --build-arg GROUP_ID=$(id -g) -t $IMAGE_NAME .
fi

# Check if the Docker container exists
CONTAINER=$(docker ps -aq -f name=$CONTAINER_NAME)
if [ -z "$CONTAINER" ]; then
  # If the container does not exist, create and start it
  echo "Docker container not found. Creating and starting container..."
  docker run --rm -v $(pwd):/work -di --name $CONTAINER_NAME $IMAGE_NAME
 fi

echo "Running docker exec on the container..."
docker exec -it $CONTAINER_NAME bash -c "/work/scripts/build.sh $*"
