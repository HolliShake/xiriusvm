@echo off

clang -c .\\src\\context.c -o context.o
clang -c .\\src\\global.c -o global.o
clang -c .\\src\\opcode.c -o opcode.o
clang -c .\\src\\runtime.c -o runtime.o
clang -c .\\src\\store.c -o store.o
clang -c .\\src\\xsvalue.c -o xsvalue.o

ar rcs libxiriusvm.a context.o global.o opcode.o runtime.o store.o xsvalue.o
del ".\\context.o"
del ".\\global.o"
del ".\\opcode.o"
del ".\\runtime.o"
del ".\\store.o"
del ".\\xsvalue.o"