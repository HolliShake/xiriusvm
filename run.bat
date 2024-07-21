@echo off

clang main.c .\\src\\*.c -lm -Wall -o main.exe
.\\main.exe