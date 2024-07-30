@echo off

clang -fPIC .\\virtualmachine\\xirius.c .\\virtualmachine\\*.c -lm -ltcc -shared -o libxiriusvm.dll