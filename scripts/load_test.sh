#!/usr/bin/env bash

PORT="3490"

# Run ApacheBench inside Docker to benchmark the server
# -n 50000  => total number of HTTP requests to send
# -c 400    => number of concurrent requests to make at the same time
docker run --rm --network host jordi/ab -n 30000 -c 300 http://localhost:$PORT/