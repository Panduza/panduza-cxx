#!/bin/bash

# Check if the Docker container exists
CONTAINER=$(docker ps -aq -f name=pza-libcxx)
if [ -z "$CONTAINER" ]; then
  # If the container does not exist, create and start it
  echo "Docker container not found. Running container..."
  docker run --rm --network=host -v $(pwd):/work -di --name pza-libcxx pza-cpp-img
 fi

echo "Running docker exec on the container..."
docker exec -it pza-libcxx bash -c "/work/$*"
