@echo off

clang -c .\\src\\context.c -o context.o
clang -c .\\src\\environment.c -o environment.o
clang -c .\\src\\global.c -o global.o
clang -c .\\src\\object.c -o object.o
clang -c .\\src\\opcode.c -o opcode.o
clang -c .\\src\\runtime.c -o runtime.o
clang -c .\\src\\store.c -o store.o
clang -c .\\src\\value.c -o value.o

ar rcs libxiriusvm.a context.o global.o object.o opcode.o runtime.o store.o value.o
del ".\\context.o"
del ".\\environment.o"
del ".\\global.o"
del ".\\object.o"
del ".\\opcode.o"
del ".\\runtime.o"
del ".\\store.o"
del ".\\value.o"