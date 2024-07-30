@echo off

clang -c .\\virtualmachine\\context.c -o context.o
clang -c .\\virtualmachine\\environment.c -o environment.o
clang -c .\\virtualmachine\\global.c -o global.o
clang -c .\\virtualmachine\\object.c -o object.o
clang -c .\\virtualmachine\\opcode.c -o opcode.o
clang -c .\\virtualmachine\\operation.c -o operation.o
clang -c .\\virtualmachine\\runtime.c -o runtime.o
clang -c .\\virtualmachine\\store.c -o store.o
clang -c .\\virtualmachine\\value.c -o value.o

ar rcs libxiriusvm.a context.o global.o object.o opcode.o runtime.o store.o value.o
del ".\\context.o"
del ".\\environment.o"
del ".\\global.o"
del ".\\object.o"
del ".\\opcode.o"
del ".\\operation.o"
del ".\\runtime.o"
del ".\\store.o"
del ".\\value.o"