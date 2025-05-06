TITLE HLWR Build
@ECHO off
COLOR 0A
CLS

GOTO MAIN

:MAIN
cmake --build build --config Debug
pause