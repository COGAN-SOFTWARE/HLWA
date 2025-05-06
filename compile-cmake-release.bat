TITLE HLWA Build
@ECHO off
COLOR 0A
CLS

GOTO MAIN

:MAIN
cmake --build build --config Release
pause