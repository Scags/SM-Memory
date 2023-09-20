@echo off
SET arg=%1

:: Change these if building yourself
SET mms_path="C:/Users/johnm/Documents/GitHub/metamod-source"
SET sm_path="C:/Users/johnm/Documents/GitHub/sourcemod"

:: Allow passing a debug flag for debug builds
SET pyflag=--enable-optimize
IF "%arg%"=="--debug" SET pyflag=--enable-debug

py ../configure.py --mms-path %mms_path% --sm-path %sm_path% --sdks none %pyflag%
ambuild
