TITLE HLWR Build
@ECHO off
COLOR 0A
CLS

GOTO MAIN

:MAIN
cmake -B build -S . -G "Visual Studio 17 2022" -T ClangCL
pause