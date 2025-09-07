#!/usr/bin/env bash

PORT="3490"

# Build the Docker image of the server
docker build -t "http-server:latest" .

# Run the server container, mapping container port 3490 to host port 3490
docker run --rm -it -p $PORT:$PORT "http-server:latest"