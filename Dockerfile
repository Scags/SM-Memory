FROM debian:10-slim

RUN apt-get update && apt-get install -y \
	git \
	python3 \
	python3-pip \
	gcc \
	g++ \
	g++-multilib \
&& rm -rf /var/lib/apt/lists/*

# Clone SM stuff
WORKDIR /dependencies
RUN git clone -b master https://github.com/alliedmodders/metamod-source.git --depth 1
RUN git clone -b master https://github.com/alliedmodders/sourcemod.git --recursive --depth 1
RUN git clone -b master https://github.com/alliedmodders/ambuild.git --depth 1

WORKDIR /dependencies/ambuild
RUN python3 -m pip install .

WORKDIR /project

# Copy the current directory contents into the container at /project
COPY . .

WORKDIR /project/package

ARG BUILD_MODE=optimize
ENV PY_ARG=--enable-$BUILD_MODE

# Build
RUN python3 ../configure.py \
	--mms-path "/dependencies/metamod-source" \
	--sm-path "/dependencies/sourcemod" \
	$PY_ARG

RUN ambuild
