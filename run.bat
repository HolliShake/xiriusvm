@echo off

clang main.c .\\src\\*.c -lm -o main.exe
.\\main.exe