@echo off

clang -fPIC .\\src\\xirius.c .\\src\\*.c -lm -shared -o libxiriusvm.dll