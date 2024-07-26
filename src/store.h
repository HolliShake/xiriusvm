#include "global.h"

#ifndef STORE_H
#define STORE_H
    #ifndef OPCODE_H
        /*virtual*/ typedef struct xirius_instruction_struct XS_instruction;
    #endif
    
    typedef struct store_struct {
        size_t scount;
        char** stack; 
        size_t icount;
        XS_instruction** instructions;
    } store_t;

    EXPORT store_t* store_new();
    EXPORT void store_emit_line(store_t* store, char* origin, size_t line, size_t colm);
    EXPORT void store_push(store_t* store, XS_instruction* instruction);
#endif