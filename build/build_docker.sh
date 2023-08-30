#!/usr/bin/env bash

docker build -f ../Dockerfile -t sm-mem ../
container_id=$(docker create sm-mem)
docker cp "${container_id}:/project/package/package" .
docker rm ${container_id}
docker image prune -f