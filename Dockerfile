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
RUN git clone -b master https://github.com/alliedmodders/metamod-source.git
RUN git clone -b master https://github.com/alliedmodders/sourcemod.git --recursive
RUN git clone -b master https://github.com/alliedmodders/ambuild.git

WORKDIR /dependencies/ambuild
RUN python3 -m pip install .

WORKDIR /project

# Copy the current directory contents into the container at /project
COPY . .

WORKDIR /project/package

# Build
RUN python3 ../configure.py \
	--mms-path "/dependencies/metamod-source" \
	--sm-path "/dependencies/sourcemod" \
	--enable-optimize
RUN ambuild
