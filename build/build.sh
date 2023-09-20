#!/usr/bin/env bash

# Change these if building yourself
mms_path="/mnt/c/Users/johnm/Documents/GitHub/metamod-source"
sm_path="/mnt/c/Users/johnm/Documents/GitHub/sourcemod"

py_arg="--enable-optimize"
if [ "$1" == "--debug" ]; then
    py_arg="--enable-debug"
fi

python3 ../configure.py --mms-path $mms_path --sm-path $sm_path $py_arg
ambuild
