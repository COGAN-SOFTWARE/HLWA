TITLE HLWR Build
@ECHO off
COLOR 0A
CLS

GOTO MAIN

:MAIN
cmake -S . -B build -G "Ninja" -DCMAKE_CXX_COMPILER=clang++ -DCMAKE_C_COMPILER=clang
pause